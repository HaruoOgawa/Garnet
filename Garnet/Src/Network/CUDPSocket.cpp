#ifdef USE_NETWORK
#include "CUDPSocket.h"
#include "../AppCore/CApp.h"
#include "../Message/Console.h"

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
		char buffer[1024];
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

		// データ受信時の処理
		Console::Log("UDP Recieve Data: %s\n", buffer);

		return true;
	}
}
#endif // USE_NETWORK