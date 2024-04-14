#ifdef USE_WIN32_WindowAPI

#include "CDemoAppManager.h"
#include "../LoadWorker/CLoadWorker.h"
#ifdef USE_OPENGL
#include "../GraphicsAPI/OpenGL/COpenGLAPI.h"
#endif
#include "./ScriptApp/CScriptApp.h"

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#define WGL_IMPLEMENTATION

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

namespace app
{
	// 仮のグローバル変数
	bool g_IsRunLoop = true;
	CDemoAppManager* g_AppManager = nullptr;

	CDemoAppManager::CDemoAppManager(app::EAppType AppType):
		m_Window(nullptr),
		m_Device_Context(nullptr),
		m_Rendering_Context(nullptr),
		m_IsRunLoop(true),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
		m_LoadWorker(nullptr),
		m_InputState(std::make_shared<input::CInputState>(1.0f)),
		m_GraphicsAPI(nullptr),
		m_App(nullptr),
		m_GUIEngine(nullptr)
	{
		g_AppManager = this; // 仮のグローバル変数

#ifdef USE_OPENGL
		m_GraphicsAPI = std::make_shared<api::COpenGLAPI>(WIDTH, HEIGHT);
#endif

		m_App = std::make_shared<app::CScriptApp>();

#ifdef USE_GUIENGINE
		m_GUIEngine = std::make_shared<gui::CImGuiGUIEngine>();
#else
		m_GUIEngine = std::make_shared<gui::CDummyGUIEngine>();
#endif
	}

	CDemoAppManager::~CDemoAppManager()
	{
		if (m_App)
		{
			m_App->Release(m_GraphicsAPI.get());
			m_App.reset();
			m_App = nullptr;
		}

		if (m_LoadWorker)
		{
			m_LoadWorker.reset();
			m_LoadWorker = nullptr;
		}

		if (m_GUIEngine)
		{
			m_GUIEngine->Release(m_GraphicsAPI.get());
			m_GUIEngine.reset();
			m_GUIEngine = nullptr;
		}

		if (m_GraphicsAPI)
		{
			m_GraphicsAPI->Release();
			m_GraphicsAPI.reset();
			m_GraphicsAPI = nullptr;
		}

		if (m_Rendering_Context)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_Rendering_Context);
		}

		ReleaseDC(m_Window,m_Device_Context);

		g_AppManager = nullptr;
	}

	const std::shared_ptr<gui::IGUIEngine>& CDemoAppManager::GetGUIEngine() const
	{
		return m_GUIEngine;
	}

	bool CDemoAppManager::Initialize(HINSTANCE hInstance)
	{
		if (!InitWindow(hInstance)) return false;
		//if (!InitWGL()) return false;
		if (!InitGLContext()) return false;
		if (!m_GraphicsAPI->Initialize()) return false;

#ifdef USE_GUIENGINE
		if (!m_GUIEngine->InitializeWithWin32API(m_Window, m_GraphicsAPI.get())) return false;
#endif

		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(m_GraphicsAPI.get());

		if (!m_App->Initialize(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		RECT rect;
		if (GetWindowRect(m_Window, &rect))
		{
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			m_GraphicsAPI->Resize(w, h);
			m_App->Resize(w, h);
		}

		return true;
	}

	/*void Resize_Callback(GLFWwindow* window, int width, int height)
	{
		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
		AppManager->ResizeWindow(width, height);
	}*/

	void KeyCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param, bool IsDown)
	{
		if (w_param < 256)
		{
			// WPARAM Key Codes
			// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
			if (w_param == VK_ESCAPE)
			{
				g_IsRunLoop = false;
			}

			if (!g_AppManager) return;

			auto AppManager = g_AppManager;

#ifdef USE_INPUT_SYSTEM
			auto InputState = AppManager->GetInputState();

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
		if (!g_AppManager) return;

		auto AppManager = g_AppManager;
		auto InputState = AppManager->GetInputState();

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
		if (!g_AppManager) return;

		auto AppManager = g_AppManager;
		auto InputState = AppManager->GetInputState();

		if (InputState->IsDownMouseLeft() || InputState->IsDownMouseRight())
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
		if (!g_AppManager) return;

		auto AppManager = g_AppManager;
		auto InputState = AppManager->GetInputState();

		auto Amount = GET_WHEEL_DELTA_WPARAM(w_param);

		// winuser.hだとピクセルに基づくホイール量が -120 ~ 120の範囲で返ってくるのでひとまず -1.0 ~ 1.0fにしておく
		float wheelRate = glm::sign(static_cast<float>(Amount)) * 1.0f;
		InputState->SetWheelScrollAmount(glm::vec2(0.0f, wheelRate));
	}
#endif

	// ウィンドウのコールバック関数
	LRESULT MainWindowCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		if (!g_AppManager) return false;

		auto AppManager = g_AppManager;
		
		auto GUIEngine = AppManager->GetGUIEngine();
		if (GUIEngine)
		{
			if (GUIEngine->CheckInput(window, msg, w_param, l_param)) return true;
		}

		LRESULT result = 0;

		// インプット
		switch (msg)
		{

			case WM_KEYDOWN : 
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
#endif
			default:
				break;
		}

		return true;
	}

	bool CDemoAppManager::RunLopp()
	{
		m_IsRunLoop = g_IsRunLoop;

		if (m_IsRunLoop)
		{
			// Windows Message Handling(Send msg to MainWindowCallback)
			{
				MSG msg;
				if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageA(&msg);
				}
			}

			//
			if (!Update()) return false;
			if (!LateUpdate()) return false;
			if (!FixedUpdate()) return false;
			if (!Draw()) return false;

			m_InputState->Clear();
		}

		return true;
	}

	void CDemoAppManager::ResizeWindow(int w, int h)
	{
		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);
	}

	bool CDemoAppManager::InitWindow(HINSTANCE hInstance)
	{
		/// <summary>
		/// ウィンドウの設定
		/// </summary>
		/// <returns></returns>
		WNDCLASSA window_class = {}; 
		
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
		// CS_HREDRAW : 移動またはサイズ調整によってクライアント領域の幅が変化した場合、ウィンドウ全体を再描画します。
		// CS_VREDRAW : 移動またはサイズ調整によってクライアント領域の高さが変化した場合、ウィンドウ全体を再描画します。
		// CS_OWNDC   : クラス内の各ウィンドウに一意のデバイス コンテキストを割り当てます。
		window_class.lpfnWndProc = MainWindowCallback; // ウィンドウのコールバック関数
		window_class.hInstance = hInstance; // アプリのインスタンス
		//window_class.hIcon = ""; // ウィンドウのアイコン(?)ひとまず今は要らない
		window_class.lpszClassName = "GarnetWindowClass"; // WindosClassの名前. たぶんVulkanとかでいうラベルみたいなやつだと思う

		if (!RegisterClassA(&window_class)) // WindowClassを登録する
		{
			Console::Log("[Error] could not regist WindowClass\n");

			return false;
		}

		// ウィンドウを生成
		m_Window = CreateWindowExA(
			0, // WindowStyleの拡張
			window_class.lpszClassName, // WindowClassの名前. 先ほど登録しておいたもの
			"Garnet", // WindowName
			// WindowStyle : https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
			WS_POPUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE | WS_CAPTION, // WindosStyle. たぶんWindowに出てくるボタンとかタブの設定
			200, // 位置 X (適当な値)
			200, // 位置 Y (適当な値)
			WIDTH,         // Width
			HEIGHT,        // HEIGHT
			0,             // Window Parent. ウィンドウを複数個作ってグループ化できるのかな？ 例えばUnityのGame Viewと Scene ViewがあってUnityエディタ全体を動かすとそれもついてくるみたいな
			0,             // Menu(?)
			hInstance,     // アプリのインスタンス
			0              // lpParam(?)
		);

		if (!m_Window)
		{
			Console::Log("[Error] Failed to create window\n");

			return false;
		}

		// スクリーンサイズを取得
		SystemParametersInfo(SPI_GETWORKAREA, 0, &m_WorkArea, 0);

#ifdef _DEBUG
		RECT rect;
		if (GetWindowRect(m_Window, &rect))
		{
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;

			SetWindowPos(m_Window, HWND_TOP, rect.left, rect.top, w, h, NULL);
		}
#else
		// Full Screen
		int workAreaWidth = m_WorkArea.right - m_WorkArea.left;
		int workAreaHeight = m_WorkArea.bottom - m_WorkArea.top;

		SetWindowPos(m_Window, HWND_TOP, m_WorkArea.left, m_WorkArea.top, workAreaWidth, workAreaHeight, NULL);
#endif // _DEBUG

		return true;
	}

	bool CDemoAppManager::InitGLContext()
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
			Console::Log("[Error] Failed to choose Pixel Format\n");

			return false;
		}

		if (!SetPixelFormat(m_Device_Context, pixel_format, &pixel_format_desc))
		{
			Console::Log("[Error] Failed to set Pixel Format\n");

			return false;
		}

		// RenderingContextを作成
		/*int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_PROFILE_MASK_ARB,
			0
		};
		m_Rendering_Context = wglCreateContextAttribsARB(m_Device_Context, 0, attribs);*/

		m_Rendering_Context = wglCreateContext(m_Device_Context);
		wglMakeCurrent(m_Device_Context, m_Rendering_Context);

		return true;
	}

	bool CDemoAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		m_App->GetDrawInfo()->SetSecondsTime(m_SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(m_DeltaSecondsTime);

		// ViewCameraのUpdate
		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_DeltaSecondsTime, m_InputState);

		if (!m_App->Update(m_GraphicsAPI.get(), m_LoadWorker.get(), m_InputState)) return false;

		return true;
	}

	bool CDemoAppManager::LateUpdate()
	{
		if (!m_App->LateUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CDemoAppManager::FixedUpdate()
	{
		if (!m_App->FixedUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CDemoAppManager::Draw()
	{
		// Appの描画
		if (!m_App->Draw(m_GraphicsAPI.get(), m_LoadWorker.get(), m_GUIEngine)) return false;

		//カラーバッファを入れ替える
		SwapBuffers(m_Device_Context);

		return true;
	}
}
#endif // USE_WIN32_WindowAPI