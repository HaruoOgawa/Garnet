#pragma once

#ifdef USE_NETWORK

#include <stdio.h>
#include <string>
#include <WinSock2.h> // WindowsのUDPとかのSocket通信ライブラリ
#include <Windows.h>
#include <windowsx.h> // Windowsのメッセージ処理とかに必要
#include <memory>

namespace app { class CApp; }
namespace binary { class CBinaryReader; }

namespace network
{
	class CUDPSocket
	{
		const std::string m_Address;
		const int m_Port;

		int m_Socket;

		std::shared_ptr<app::CApp> m_App;

		long long m_WaitTimeMillSeconds;
	private:
		void Receive();

		bool AnalyseData(app::CApp* pApp, binary::CBinaryReader& Analyser);
		bool AnalyseArtNet(app::CApp* pApp, binary::CBinaryReader& Analyser);

		static bool DecomposeAbsoluteUniverse(unsigned short AbsoluteUniverse, unsigned short& Net, unsigned short& SubNet, unsigned short& Universe);

	public:
		CUDPSocket(const std::string& Address, int Port);
		virtual ~CUDPSocket();

		void Close();

		bool Initialize(const std::shared_ptr<app::CApp>& App, bool MultiThread = false, long long WaitTimeMillSeconds = 0.0);

		bool Update(app::CApp* pApp);
	};
}
#endif // USE_NETWORK