#ifdef USE_NETWORK
#include "CUDPSocket.h"
#include "../AppCore/CApp.h"
#include "../Message/Console.h"
#include "../Binary/CBinaryReader.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) // inet_addr()関数で警告が出る場合は以下で警告を無効化する。

namespace network
{
	CUDPSocket::CUDPSocket(const std::string& Address, int Port):
		m_Address(Address),
		m_Port(Port),
		m_Socket(INVALID_SOCKET)
	{
	}

	CUDPSocket::~CUDPSocket()
	{
		Close();
	}

	bool CUDPSocket::Initialize()
	{
		WSAData wsaData;

		// MAKEWORD(2, 0) はWinSocketのバージョン
		WSAStartup(MAKEWORD(2, 0), &wsaData);

		// AF_INETはIPv4、SOCK_DGRAMはUDP、IPPROTO_UDPはUDPプロトコルを指定
		m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		 // ノンブロッキングモードに設定
		u_long mode = 1; // 1: ノンブロッキング
		ioctlsocket(m_Socket, FIONBIO, &mode);

		// アドレス指定
		sockaddr_in addr;
		addr.sin_family = AF_INET; // IPv4
		addr.sin_port = htons(m_Port); // ポート番号を設定
		//addr.sin_addr.s_addr = inet_addr(m_Address.c_str()); // IPアドレスを設定。INADDR_ANYは全てのアドレスからパケットを受け取る
		addr.sin_addr.s_addr = INADDR_ANY; // IPアドレスを設定

		if (addr.sin_addr.s_addr == INADDR_NONE) {
			Console::Log("Invalid IP address: %s\n", m_Address.c_str());
			return false;
		}

		// バインドする
		if (bind(m_Socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
			// バインドに失敗した原因を表示
			// エラーコードを取得
			int errorCode = WSAGetLastError();
			if (errorCode == WSAEADDRINUSE) {
				Console::Log("Address already in use: %s:%s\n", m_Address.c_str(), std::to_string(m_Port).c_str());
			} else if (errorCode == WSAEACCES) {
				Console::Log("Permission denied for address: %s:%s\n", m_Address.c_str(), std::to_string(m_Port).c_str());
			} else {
				Console::Log("Failed to bind UDP socket: %d\n", errorCode);
			}

			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;

			Console::Log("Failed to bind UDP socket to address: %s:%s\n", m_Address.c_str(), std::to_string(m_Port).c_str());

			return false;
		}

		return true;
	}

	void CUDPSocket::Close()
	{
		// ソケットが有効な場合は閉じる
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			Console::Log("UDP socket closed.\n");
		}

		WSACleanup(); // WinSockのクリーンアップ
	}

	bool CUDPSocket::Update(app::CApp* pApp)
	{
		const int BufferSize = 1024; // 受信バッファのサイズを指定

		char buffer[BufferSize];
		std::memset(buffer, 0, sizeof(buffer));
		sockaddr_in senderAddr;
		int senderAddrSize = sizeof(senderAddr);

		int bytesReceived = recvfrom(m_Socket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&senderAddr, &senderAddrSize);

		if (bytesReceived == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK) {
				// データが来ていない場合は何もしない
				return true;
			} else {
				Console::Log("UDP recvfrom error: %d\n", err);
				return false;
			}
		}

		std::vector<unsigned char> ReadData;
		ReadData.resize(BufferSize);
		std::memcpy(ReadData.data(), buffer, 1024);

		// データ受信時の処理
		binary::CBinaryReader Analyser(ReadData);
		if (!AnalyseData(pApp, Analyser)) return false;
		//Console::Log("UDP Recieve Data: %s\n", buffer);

		return true;
	}

	bool CUDPSocket::AnalyseData(app::CApp* pApp, binary::CBinaryReader& Analyser)
	{
		std::string NetworkProtocol = std::string();
		if (!Analyser.GetStringToZeroByte(NetworkProtocol)) return false;

		if (NetworkProtocol == "Art-Net")
		{
			if (!AnalyseArtNet(pApp, Analyser)) return false;
		}

		return true;
	}

	bool CUDPSocket::AnalyseArtNet(app::CApp* pApp, binary::CBinaryReader& Analyser)
	{
		// 伝送プロトコルは何か(Art-Netの中に何のデータが入っているか)
		unsigned short OpCode = 0;
		if (!Analyser.GetUShort(OpCode)) return false;

		// プロトコルバージョン
		unsigned short ProtocolVersion = 0;
		if (!Analyser.GetUShort(ProtocolVersion)) return false;

		// パケット順序制御
		unsigned char Sequence = 0;
		if (!Analyser.GetByte(Sequence)) return false;

		//物理ポート番号
		unsigned char Physical = 0;
		if (!Analyser.GetByte(Physical)) return false;

		// 出力先ユニバース番号(AbsoluteUniverse: ユニバース番号の絶対値)
		// ユニバースは簡単にいうとこの信号をどの機材に渡すかどうかを判別するためのラベルのようなもの
		// Art-Netのバイナリで届くときは絶対値表記されていて0 から 32767 の間の数値が入っているが
		// Art-Netの仕様としては、「Net: 0 ～ 127」「SubNet: 0 ～ 15」「Universe: 0 ～ 15」で3つの大きな単位に分かれている
		// Netの中にSubNetがあり、SubNetの中にUniverseがある
		// これらの数値を計算すると、 128 x 16 x 16 で 32768 となる → この3つを計算したのがAbsoluteUniverse
		// https://qiita.com/LUDO/items/eec489555ecf3a872197#%E3%83%A6%E3%83%8B%E3%83%90%E3%83%BC%E3%82%B9%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6
		unsigned short AbsoluteUniverse = 0;
		if (!Analyser.GetUShort(AbsoluteUniverse)) return false;

		// AbsoluteUniverseをNet・SubNet・Universeに分解する
		unsigned short Net = 0;
		unsigned short SubNet = 0;
		unsigned short Universe = 0;

		if (!DecomposeAbsoluteUniverse(AbsoluteUniverse, Net, SubNet, Universe)) return false;

		// DMXは513バイトのバイナリ
		// 1バイト目はデータサイズで512が入っている
		// 残りの512バイトがDMX本体
		// DMXは1チャンネル1バイトで0～255の間の整数をとる
		// つまりTDから浮動小数点を渡そうとすると0に丸め込まれるので必ず0から255までの整数を指定することに注意
		// ちなみにもしかすると4バイト分使ってその整数をいい感じに使えば、小数も表現できるかもしれない
		// (このテクニックはカメラ制御に使えるかも。まぁ現場では本当はそんな使い方しないんだろうけど、勉強がてらね)
		
		// データ長
		unsigned short DataLength = 0;
		if (!Analyser.GetUShortReverse(DataLength)) return false;

		// データ本体
		std::vector<unsigned char> DataBuffer;
		DataBuffer.resize(DataLength);
		if (!Analyser.GetBinary(0, DataBuffer, DataLength)) return false;

		if (OpCode == 0x5000)
		{
			// ArtDMXパケットの処理
			Console::Log("Received ArtDMX packet: OpCode=0x%04X, ProtocolVersion=%d, Sequence=%d, Physical=%d, AbsoluteUniverse=%d, DataLength=%d\n",
				OpCode, ProtocolVersion, Sequence, Physical, AbsoluteUniverse, DataLength);
			// 受信したDMXデータをアプリケーションに通知する
			//pApp->OnArtNetDMXDataReceived(Net, SubNet, Universe, DataBuffer);
		}

		return true;
	}

	bool CUDPSocket::DecomposeAbsoluteUniverse(unsigned short AbsoluteUniverse, unsigned short& Net, unsigned short& SubNet, unsigned short& Universe)
	{
		Net = (AbsoluteUniverse >> 8) & 0x7f;
		SubNet = (AbsoluteUniverse >> 4) & 0x0f;
		Universe = (AbsoluteUniverse) & 0x0f;

		return true;
	}
}
#endif // USE_NETWORK