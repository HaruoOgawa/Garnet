#ifdef USE_OPENGL

#include "CDemoAppManager.h"
#include "../GraphicsAPI/OpenGL/COpenGLAPI.h"
#include "./ScriptApp/CScriptApp.h"
#include "./EditorApp/CEditorApp.h"
#include "./MainApp/CMainApp.h"

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

namespace app
{
	// 仮のグローバル変数
	bool g_IsRunLoop = true;

	CDemoAppManager::CDemoAppManager(app::EAppType AppType):
		m_Window(nullptr),
		m_Device_Context(nullptr),
		m_Rendering_Context(nullptr),
		m_IsRunLoop(true),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
#ifdef USE_INPUT_SYSTEM
		m_InputState(std::make_shared<input::CInputState>(1.0f)),
#endif
		m_GraphicsAPI(std::make_shared<api::COpenGLAPI>(WIDTH, HEIGHT)),
		m_App(nullptr)
	{
		if (AppType == app::EAppType::ScriptApp)
		{
			m_App = std::make_shared<app::CScriptApp>();
		}
		else if (AppType == app::EAppType::EditorApp)
		{
			m_App = std::make_shared<app::CEditorApp>();
		}
		else if (AppType == app::EAppType::MainApp)
		{
			m_App = std::make_shared<app::CMainApp>();
		}
	}

	CDemoAppManager::~CDemoAppManager()
	{
		if (m_App)
		{
			m_App->Release(m_GraphicsAPI.get());
			m_App.reset();
			m_App = nullptr;
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
	}

	bool CDemoAppManager::Initialize(HINSTANCE hInstance)
	{
		if (!InitWindow(hInstance)) return false;
		if (!InitGLContext()) return false;
		if (!m_GraphicsAPI->Initialize()) return false;

		if (!m_App->Initialize(m_GraphicsAPI.get())) return false;

		int w = WIDTH, h = HEIGHT; // 仮で適当な値を渡しておく

		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);

		return true;
	}

	// ウィンドウのコールバック関数
	LRESULT MainWindowCallback(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		LRESULT result = 0;

		// インプット
		switch (msg)
		{
		case WM_KEYDOWN : 
		{
			if (w_param < 256)
			{
				// WPARAM Key Codes
				// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
				if (w_param == VK_ESCAPE)
				{
					g_IsRunLoop = false;
				}
			}
		}
			break;
		default:
			break;
		}

		return true;
	}

//	void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//	{
//		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
//
//		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		{
//			AppManager->SetRunLoop(false);
//		}
//	}
//
//	void Resize_Callback(GLFWwindow* window, int width, int height)
//	{
//		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
//		AppManager->ResizeWindow(width, height);
//	}
//
//	void Close_Callback(GLFWwindow* window)
//	{
//		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
//		AppManager->SetRunLoop(false);
//	}
//
//#ifdef USE_INPUT_SYSTEM
//	void MousebuttonCallback(GLFWwindow* window, int button, int action, int mods)
//	{
//		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
//		auto InputState = AppManager->GetInputState();
//
//		if (button == GLFW_MOUSE_BUTTON_LEFT)
//		{
//			InputState->SetDownMouseLeft((action != GLFW_RELEASE));
//
//			// 位置を正規化する
//			double PosX, PosY;
//			glfwGetCursorPos(window, &PosX, &PosY);
//
//			int w, h;
//			glfwGetWindowSize(window, &w, &h);
//
//			float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
//			float rPosY = static_cast<float>(PosY) / static_cast<float>(h);
//
//			rPosX = rPosX * 2.0f - 1.0f;
//			rPosY = rPosY * 2.0f - 1.0f;
//
//			InputState->StartMousePos(glm::vec2(rPosX, rPosY));
//		}
//	}
//
//	void CursorPosCallback(GLFWwindow* window, double PosX, double PosY)
//	{
//		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
//		auto InputState = AppManager->GetInputState();
//
//		if (InputState->IsDownMouseLeft())
//		{
//			// 位置を正規化する
//			int w, h;
//			glfwGetWindowSize(window, &w, &h);
//
//			float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
//			float rPosY = static_cast<float>(PosY) / static_cast<float>(h);
//
//			rPosX = rPosX * 2.0f - 1.0f;
//			rPosY = rPosY * 2.0f - 1.0f;
//
//			InputState->SetMousePos(glm::vec2(rPosX, rPosY));
//		}
//	}
//#endif

	bool CDemoAppManager::RunLopp()
	{
		m_IsRunLoop = g_IsRunLoop;

		if (m_IsRunLoop)
		{
			if (!Update()) return false;
			if (!Draw()) return false;
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
			CW_USEDEFAULT, // 位置 X
			CW_USEDEFAULT, // 位置 Y
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
		m_Rendering_Context = wglCreateContext(m_Device_Context);
		wglMakeCurrent(m_Device_Context, m_Rendering_Context);

		return true;
	}

	bool CDemoAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		// ViewCameraのUpdate
#ifdef USE_INPUT_SYSTEM
		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_SecondsTime, m_InputState);
#endif // USE_INPUT_SYSTEM

		if (!m_App->Update(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		return true;
	}

	bool CDemoAppManager::Draw()
	{
		// Appの描画
		if (!m_App->Draw(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		//カラーバッファを入れ替える
		SwapBuffers(m_Device_Context);

		return true;
	}
}
#endif // USE_OPENGL