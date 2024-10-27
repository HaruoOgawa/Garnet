#ifdef USE_WIN32_WindowAPI

#include "CWin32WindowAPI.h"

#include "../AppCore/CAppCore.h"

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#define WGL_IMPLEMENTATION

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

namespace window
{
	// 仮のグローバル変数
	CWin32WindowAPI* g_WindowAPI = nullptr;

	CWin32WindowAPI::CWin32WindowAPI():
		m_pCAppCore(nullptr),
		m_Window(nullptr),
		m_Device_Context(nullptr),
		m_Rendering_Context(nullptr)
	{
		g_WindowAPI = this; // 仮のグローバル変数
	}

	bool CWin32WindowAPI::Release()
	{
		if (m_Rendering_Context)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_Rendering_Context);
		}

		ReleaseDC(m_Window, m_Device_Context);

		g_WindowAPI = nullptr;

		return true;
	}

	const HWND& CWin32WindowAPI::GetWin32Window() const
	{
		return m_Window;
	}

	bool CWin32WindowAPI::Initialize(app::CAppCore* pAppCore, app::SAppSettings Settings)
	{
		m_pCAppCore = pAppCore;

		HINSTANCE hInstance = GetModuleHandle(NULL);
		
		if (!InitWindow(hInstance, Settings)) return false;
		//if (!InitWGL()) return false;
		if (!InitGLContext()) return false;

		return true;
	}

	void ResizeCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		if (!g_WindowAPI) return;

		auto WindowAPI = g_WindowAPI;

		RECT rect;
		GetWindowRect(window, &rect);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		WindowAPI->ResizeWindow(width, height);
	}

	void KeyCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param, bool IsDown)
	{
		if (w_param < 256)
		{
			if (!g_WindowAPI) return;

			auto WindowAPI = g_WindowAPI;

			auto AppCore = WindowAPI->GetAppCore();
			if (!AppCore) return;

			// WPARAM Key Codes
			// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
			if (w_param == VK_ESCAPE)
			{
				AppCore->SetRunLoop(false);
			}

#ifdef USE_INPUT_SYSTEM
			auto InputState = AppCore->GetInputState();

			//
			input::EKeyType KeyType = input::EKeyType::KEY_TYPE_NONE;

			switch (w_param)
			{
			case 0x57:
				KeyType = input::EKeyType::KEY_TYPE_W;
				break;
			case 0x41:
				KeyType = input::EKeyType::KEY_TYPE_A;
				break;
			case 0x53:
				KeyType = input::EKeyType::KEY_TYPE_S;
				break;
			case 0x44:
				KeyType = input::EKeyType::KEY_TYPE_D;
				break;
			case 0x31:
				KeyType = input::EKeyType::KEY_TYPE_1;
				break;
			case 0x32:
				KeyType = input::EKeyType::KEY_TYPE_2;
				break;
			case 0x33:
				KeyType = input::EKeyType::KEY_TYPE_3;
				break;
			case 0x34:
				KeyType = input::EKeyType::KEY_TYPE_4;
				break;
			case 0x35:
				KeyType = input::EKeyType::KEY_TYPE_5;
				break;
			case 0x70:
				KeyType = input::EKeyType::KEY_TYPE_F1;
				break;
			case VK_CONTROL:
				KeyType = input::EKeyType::KEY_TYPE_CONTROL;
				break;
			case VK_SPACE:
				KeyType = input::EKeyType::KEY_TYPE_SPACE;
				break;

			case VK_RETURN:
				KeyType = input::EKeyType::KEY_TYPE_ENTER;
				break;

			default:
				break;
			}

			InputState->SetKeyState(KeyType, IsDown);
#endif
		}
	}
	
#ifdef USE_INPUT_SYSTEM
	void MousebuttonCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param, bool IsDown)
	{
		if (!g_WindowAPI) return;

		auto WindowAPI = g_WindowAPI;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		auto InputState = AppCore->GetInputState();

		if ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) && !InputState->IsDownMouseRight())
		{
			InputState->SetDownMouseLeft(IsDown);
		}
		else if ((msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) && !InputState->IsDownMouseLeft())
		{
			InputState->SetDownMouseRight(IsDown);
		}

		POINT p;
		GetCursorPos(&p);

		double PosX = (double)p.x, PosY = (double)p.y;

		RECT rect;
		GetWindowRect(window, &rect);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		// 位置を正規化する
		float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
		float rPosY = static_cast<float>(PosY) / static_cast<float>(h);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;

		InputState->StartMousePos(glm::vec2(rPosX, rPosY));
	}

	void CursorPosCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		if (!g_WindowAPI) return;

		auto WindowAPI = g_WindowAPI;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		auto InputState = AppCore->GetInputState();

		//if (InputState->IsDownMouseLeft() || InputState->IsDownMouseRight())
		{
			POINT p;
			GetCursorPos(&p);
			
			double PosX = (double)p.x, PosY = (double)p.y;

			RECT rect;
			GetWindowRect(window, &rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			// 位置を正規化する
			float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
			float rPosY = static_cast<float>(PosY) / static_cast<float>(h);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			InputState->SetMousePos(glm::vec2(rPosX, rPosY));
		}
	}

	void ScrollCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		if (!g_WindowAPI) return;

		auto WindowAPI = g_WindowAPI;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		auto InputState = AppCore->GetInputState();

		auto Amount = GET_WHEEL_DELTA_WPARAM(w_param);

		// winuser.hだとピクセルに基づくホイール量が -120 ~ 120の範囲で返ってくるのでひとまず -1.0 ~ 1.0fにしておく
		float wheelRate = glm::sign(static_cast<float>(Amount)) * 1.0f;
		InputState->SetWheelScrollAmount(glm::vec2(0.0f, wheelRate));
	}
#endif

	void FocusCallback(bool Focused)
	{
		if (!g_WindowAPI) return;

		auto WindowAPI = g_WindowAPI;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		AppCore->FocusWindow(Focused);
	}

	// ウィンドウのコールバック関数
	LRESULT MainWindowCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		auto WindowAPI = g_WindowAPI;
		
		if (WindowAPI)
		{
			auto AppCore = WindowAPI->GetAppCore();

			if (AppCore)
			{
				auto GUIEngine = AppCore->GetGUIEngine();
				if (GUIEngine)
				{
					if (GUIEngine->CheckInput(window, msg, w_param, l_param)) return DefWindowProc(window, msg, w_param, l_param);

					if (GUIEngine->IsExistMouseOnGUI()) return DefWindowProc(window, msg, w_param, l_param);
				}

				// インプット
				switch (msg)
				{
				case WM_KEYDOWN:
					KeyCallback(window, msg, w_param, l_param, true);
					break;

				case WM_KEYUP:
					KeyCallback(window, msg, w_param, l_param, false);
					break;
#ifdef USE_INPUT_SYSTEM
				case WM_LBUTTONDOWN:
					MousebuttonCallback(window, msg, w_param, l_param, true);
					break;

				case WM_RBUTTONDOWN:
					MousebuttonCallback(window, msg, w_param, l_param, true);
					break;

				case WM_LBUTTONUP:
					MousebuttonCallback(window, msg, w_param, l_param, false);
					break;

				case WM_RBUTTONUP:
					MousebuttonCallback(window, msg, w_param, l_param, false);
					break;

				case WM_MOUSEMOVE:
					CursorPosCallback(window, msg, w_param, l_param);
					break;

				case WM_MOUSEWHEEL:
					ScrollCallback(window, msg, w_param, l_param);
					break;

				case WM_SETFOCUS:
					FocusCallback(true);
					break;

				case WM_KILLFOCUS:
					FocusCallback(false);
					break;

				case WM_SIZE:
					ResizeCallback(window, msg, w_param, l_param);
					break;

				case WM_CLOSE:
					AppCore->SetRunLoop(false);
					break;
#endif
				default:
					break;
				}
			}
		}

		// 0/1でリターンとDefWindowProcが無いとシステムメニューなどが表示されなくなるので注意
		//return 0;
		return DefWindowProc(window, msg, w_param, l_param);
	}

	bool CWin32WindowAPI::InitWindow(HINSTANCE hInstance, app::SAppSettings Settings)
	{
		/// <summary>
		/// ウィンドウの設定
		/// </summary>
		/// <returns></returns>
		WNDCLASSEX window_class = {}; 
		window_class.cbSize = sizeof(WNDCLASSEX);
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
		// CS_HREDRAW : 移動またはサイズ調整によってクライアント領域の幅が変化した場合、ウィンドウ全体を再描画します。
		// CS_VREDRAW : 移動またはサイズ調整によってクライアント領域の高さが変化した場合、ウィンドウ全体を再描画します。
		// CS_OWNDC   : クラス内の各ウィンドウに一意のデバイス コンテキストを割り当てます。
		window_class.lpfnWndProc = MainWindowCallback; // ウィンドウのコールバック関数
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = hInstance; // アプリのインスタンス
		window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION); // ウィンドウのアイコン
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		window_class.lpszMenuName = NULL;
#ifdef _DEBUG
		window_class.lpszClassName = L"GarnetWindowClass"; // WindosClassの名前. たぶんVulkanとかでいうラベルみたいなやつだと思う
#else
		window_class.lpszClassName = (LPCSTR)L"GarnetWindowClass"; // WindosClassの名前. たぶんVulkanとかでいうラベルみたいなやつだと思う
#endif // _DEBUG
		window_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&window_class)) // WindowClassを登録する
		{
#ifdef _DEBUG
			Console::Log("[Error] could not regist WindowClass\n");
#endif // _DEBUG

			return false;
		}

		DWORD dwStyle = 0;
		int nWidth = 0;
		int nHeight = 0;

		if (Settings.FullScreen)
		{
			if (Settings.Overlap)
			{
				// オーバーラップ(重なり)有効
				dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			}
			else
			{
				dwStyle = WS_POPUP | WS_VISIBLE;
			}

			nWidth = GetSystemMetrics(SM_CXSCREEN);
			nHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		else
		{
			dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			nWidth = CW_USEDEFAULT;
			nHeight = CW_USEDEFAULT;
		}

#ifdef USE_WEBGPU
		std::wstring Title = L"Garnet (WebGPU)";
#elif USE_VULKAN
		std::wstring Title = L"Garnet (Vulkan)";
#elif USE_OPENGL
		std::wstring Title = L"Garnet (OpenGL)";
#endif // USE_WEBGPU

		// ウィンドウを生成
		m_Window = CreateWindowEx(
			// https://learn.microsoft.com/ja-jp/windows/win32/winmsg/extended-window-styles
			WS_EX_APPWINDOW, // WindowStyleの拡張
			window_class.lpszClassName, // WindowClassの名前. 先ほど登録しておいたもの
#ifdef _DEBUG
			Title.c_str(), // WindowName
#else
			(LPCSTR)Title.c_str(), // WindowName
#endif // _DEBUG
			// WindowStyle : https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
			dwStyle, // WindosStyle. たぶんWindowに出てくるボタンとかタブの設定
			nWidth, // 位置 X (適当な値)
			nHeight, // 位置 Y (適当な値)
			Settings.ScreenWidth,   // Width
			Settings.ScreenHeight,  // HEIGHT
			NULL,          // Window Parent. ウィンドウを複数個作ってグループ化できるのかな？ 例えばUnityのGame Viewと Scene ViewがあってUnityエディタ全体を動かすとそれもついてくるみたいな
			NULL,          // Menu(?)
			hInstance,     // アプリのインスタンス
			NULL           // lpParam(?)
		);

		if (!m_Window)
		{
#ifdef _DEBUG
			Console::Log("[Error] Failed to create window\n");
#endif

			return false;
		}

		// スクリーンサイズを取得
		if (Settings.FullScreen)
		{
			// Full Screen
			SetWindowPos(m_Window, HWND_TOP, 0, 0, nWidth, nHeight, NULL);
		}
		else
		{
			RECT rect;
			if (GetWindowRect(m_Window, &rect))
			{
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;

				SetWindowPos(m_Window, HWND_TOP, rect.left, rect.top, w, h, NULL);
			}
		}

		return true;
	}

	bool IsExtensionSupported(const char* extList, const char* extension) {
		const char* start;
		const char* where, * terminator;

		// Extension names should not have spaces.
		where = strchr(extension, ' ');
		if (where || *extension == '\0') {
			return false;
		}

		// It takes a bit of care to be fool-proof about parsing the
		// OpenGL extensions string. Don't be fooled by sub-strings, etc.
		for (start = extList;;) {
			where = strstr(start, extension);
			if (!where) {
				break;
			}
			terminator = where + strlen(extension);
			if ((where == start || *(where - 1) == ' ') &&
				(*terminator == ' ' || *terminator == '\0')) {
				return true;
			}
			start = terminator;
		}
		return false;
	}

	bool CWin32WindowAPI::InitGLContext()
	{
		// デバイスコンテキストの取得
		m_Device_Context = GetDC(m_Window); 

		// PixelFormatの設定 https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
		PIXELFORMATDESCRIPTOR pixel_format_desc = { 0 };
		pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR); // PIXELFORMATDESCRIPTORのサイズ. たぶん環境ごとにサイズが異なるのかな
		pixel_format_desc.nVersion = 1; // Version
		pixel_format_desc.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER; // ピクセル バッファーのプロパティを指定するビット フラグのセット
		// PFD_SUPPORT_OPENGL : バッファのOpenGLサポート
		// PFD_DRAW_TO_WINDOW : バッファーはウィンドウまたはデバイスのサーフェイスに描画できます。
		// PFD_DOUBLEBUFFER   : ダブルバッファであることを示す。 たぶんこれで垂直同期(SwapBuffers)が使えるようになる
		pixel_format_desc.iPixelType = PFD_TYPE_RGBA; // ピクセルタイプ
		pixel_format_desc.cColorBits = 32; // バッファデータのビット数
		pixel_format_desc.cDepthBits = 32; // バッファデータのビット数
		pixel_format_desc.dwLayerMask = PFD_MAIN_PLANE; // ??? なんか現在は使用されていないらしい？

		INT32 pixel_format = ChoosePixelFormat(m_Device_Context, &pixel_format_desc);
		if(!pixel_format)
		{
#ifdef _DEBUG
			Console::Log("[Error] Failed to choose Pixel Format\n");
#endif

			return false;
		}

		if (!SetPixelFormat(m_Device_Context, pixel_format, &pixel_format_desc))
		{
#ifdef _DEBUG
			Console::Log("[Error] Failed to set Pixel Format\n");
#endif

			return false;
		}

		// 一時的なレンダリングコンテキストを作成
		// wglCreateContextAttribsARBのProcAddressを取得するには何かしらのOpenGLコンテキストが設定されている必要がある
		HGLRC Tmp_Rendering_Context = wglCreateContext(m_Device_Context);
		wglMakeCurrent(m_Device_Context, Tmp_Rendering_Context);

		/*const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
		if (IsExtensionSupported(extensions, "WGL_ARB_create_context")) {
#ifdef _DEBUG
			Console::Log("WGL_ARB_create_context is supported.\n");
#endif
		}
		else {
#ifdef _DEBUG
			Console::Log("WGL_ARB_create_context is not supported.\n");
#endif
		}*/

		// RenderingContextを作成
		int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
		};

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		m_Rendering_Context = wglCreateContextAttribsARB(m_Device_Context, 0, attribs);

		m_Rendering_Context = wglCreateContext(m_Device_Context);

		wglMakeCurrent(m_Device_Context, m_Rendering_Context);

		// 一時的なコンテキストを削除
		wglDeleteContext(Tmp_Rendering_Context);

		return true;
	}

	void CWin32WindowAPI::SwapWindowBuffers()
	{
		//カラーバッファを入れ替える
		SwapBuffers(m_Device_Context);
	}

	void CWin32WindowAPI::AssignCurrentWindowSize()
	{
		RECT rect;
		if (GetWindowRect(m_Window, &rect))
		{
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			m_pCAppCore->ResizeWindow(w, h);
		}
	}

	void CWin32WindowAPI::PollEvents()
	{
		// Windows Message Handling(Send msg to MainWindowCallback)
		{
			MSG msg;
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}
	}

	app::CAppCore* CWin32WindowAPI::GetAppCore() const
	{
		return m_pCAppCore;
	}

	void CWin32WindowAPI::ResizeWindow(int w, int h)
	{
		m_pCAppCore->ResizeWindow(w, h);
	}

	// インプットイベント
	void CWin32WindowAPI::OnKeyDown(std::string key)
	{
	}

	void CWin32WindowAPI::OnKeyUp(std::string key)
	{
	}

	// リサイズイベント
	void CWin32WindowAPI::OnResize(int w, int h)
	{
	}

	// フォーカスイベント
	void CWin32WindowAPI::OnFocus(int focused)
	{
	}

	// マウスイベント
	void CWin32WindowAPI::OnMouseDown(int buttonNum, int x, int y)
	{
	}

	void CWin32WindowAPI::OnMouseUp(int buttonNum, int x, int y)
	{
	}

	void CWin32WindowAPI::OnMouseMove(int x, int y)
	{
	}

	void CWin32WindowAPI::OnMouseWheel(int deltaY)
	{
	}
}
#endif // USE_WIN32_WindowAPI