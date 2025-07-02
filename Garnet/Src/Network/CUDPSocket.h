#pragma once

#ifdef USE_NETWORK

#include <stdio.h>
#include <string>
#include <WinSock2.h> // WindowsのUDPとかのSocket通信ライブラリ
#include <Windows.h>
#include <windowsx.h> // Windowsのメッセージ処理とかに必要

namespace app { class CApp; }

namespace network
{
	class CUDPSocket
	{
		const std::string m_Address;
		const int m_Port;

		int m_Socket;
	private:
		void Close();

	public:
		CUDPSocket(const std::string& Address, int Port);
		virtual ~CUDPSocket();

		bool Initialize();

		bool Update(app::CApp* pApp);
	};
}
#endif // USE_NETWORK