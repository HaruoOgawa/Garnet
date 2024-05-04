#ifdef USE_GLFW

#include "CDescAppManager.h"
#include "../Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// emscripten_webgpu_get_deviceの使用に必要なインクルード
#include <emscripten/html5_webgpu.h>
#endif

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

#include "../Input/CInputState.h"

#include "CAppCore.h"

bool g_IsRunLoop = true;

namespace descapp
{
	CDescAppManager::CDescAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_AppCore(nullptr),
		m_IsRunLoop(g_IsRunLoop),
		m_SecondsTime(0.0f), 
		m_DeltaSecondsTime(0.0f)
	{
		m_AppCore = std::make_shared<app::CAppCore>();
	}

	CDescAppManager::~CDescAppManager()
	{
		Release();
	}

	GLFWwindow* CDescAppManager::GetGLFWWindow() const
	{
		return m_pWindow;
	}

	const std::shared_ptr<app::CAppCore>& CDescAppManager::GetAppCore() const
	{
		return m_AppCore;
	}

	bool CDescAppManager::Release()
	{
		if (m_AppCore)
		{
			m_AppCore->Release();
			m_AppCore.reset();
			m_AppCore = nullptr;
		}
		
		if (m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();

			m_pWindow = nullptr;
		}

		return true;
	}

	bool CDescAppManager::Initialize()
	{
		if (!InitWindow()) return false;

		if (!m_AppCore->Initialize(this)) return false;

		int w, h;
		glfwGetWindowSize(m_pWindow, &w, &h);

		m_AppCore->Resize(w, h);

		return true;
	}

	void KetCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		if (!AppManager) return;

		auto AppCore = AppManager->GetAppCore();
		if (!AppCore) return;

		auto GUIEngine = AppCore->GetGUIEngine();

		if (GUIEngine)
		{
			if (GUIEngine->IsExistMouseOnGUI()) return;
		}

#ifdef USE_INPUT_SYSTEM
		auto InputState = AppCore->GetInputState();

		//
		input::EKeyType KeyType = input::EKeyType::KEY_TYPE_NONE;
		
		switch (key)
		{
			case GLFW_KEY_W:
				KeyType = input::EKeyType::KEY_TYPE_W;
				break;
			case GLFW_KEY_A:
				KeyType = input::EKeyType::KEY_TYPE_A;
				break;
			case GLFW_KEY_S:
				KeyType = input::EKeyType::KEY_TYPE_S;
				break;
			case GLFW_KEY_D:
				KeyType = input::EKeyType::KEY_TYPE_D;
				break;
			case GLFW_KEY_1:
				KeyType = input::EKeyType::KEY_TYPE_1;
				break;
			case GLFW_KEY_2:
				KeyType = input::EKeyType::KEY_TYPE_2;
				break;
			case GLFW_KEY_3:
				KeyType = input::EKeyType::KEY_TYPE_3;
				break;
			case GLFW_KEY_4:
				KeyType = input::EKeyType::KEY_TYPE_4;
				break;
			case GLFW_KEY_5:
				KeyType = input::EKeyType::KEY_TYPE_5;
				break;
			default:
				break;
		}

		InputState->SetKeyState(KeyType, (action == GLFW_PRESS || action == GLFW_REPEAT));
#endif

		// ループ終了
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			g_IsRunLoop = false;
		}
	}

	void Resize_Callback(GLFWwindow* window, int width, int height)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		AppManager->ResizeWindow(width, height);
	}

	void Close_Callback(GLFWwindow* window)
	{
		g_IsRunLoop = false;
	}

	void MousebuttonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		if (!AppManager) return;

		auto AppCore = AppManager->GetAppCore();
		if (!AppCore) return;

		auto GUIEngine = AppCore->GetGUIEngine();

		if (GUIEngine)
		{
			if (GUIEngine->IsExistMouseOnGUI()) return;
		}

#ifdef USE_INPUT_SYSTEM
		auto InputState = AppCore->GetInputState();

		if (button == GLFW_MOUSE_BUTTON_LEFT && !InputState->IsDownMouseRight())
		{
			InputState->SetDownMouseLeft((action == GLFW_PRESS));
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && !InputState->IsDownMouseLeft())
		{
			InputState->SetDownMouseRight((action == GLFW_PRESS));
		}

		// 位置を正規化する
		double PosX, PosY;
		glfwGetCursorPos(window, &PosX, &PosY);

		int w, h;
		glfwGetWindowSize(window, &w, &h);

		float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
		float rPosY = static_cast<float>(PosY) / static_cast<float>(h);

		rPosX = rPosX * 2.0f - 1.0f;
		rPosY = rPosY * 2.0f - 1.0f;
		
		if (action == GLFW_PRESS)
		{
			InputState->StartMousePos(glm::vec2(rPosX, rPosY));
		}
#endif
	}

	void CursorPosCallback(GLFWwindow* window, double PosX, double PosY)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		if (!AppManager) return;

		auto AppCore = AppManager->GetAppCore();
		if (!AppCore) return;

		auto GUIEngine = AppCore->GetGUIEngine();

		if (GUIEngine)
		{
			if (GUIEngine->IsExistMouseOnGUI()) return;
		}

#ifdef USE_INPUT_SYSTEM
		auto InputState = AppCore->GetInputState();

		if (InputState->IsDownMouseLeft() || InputState->IsDownMouseRight())
		{
			// 位置を正規化する
			int w, h;
			glfwGetWindowSize(window, &w, &h);

			float rPosX = static_cast<float>(PosX) / static_cast<float>(w);
			float rPosY = static_cast<float>(PosY) / static_cast<float>(h);

			rPosX = rPosX * 2.0f - 1.0f;
			rPosY = rPosY * 2.0f - 1.0f;

			InputState->SetMousePos(glm::vec2(rPosX, rPosY));
		}
#endif
	}

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		if (!AppManager) return;

		auto AppCore = AppManager->GetAppCore();
		if (!AppCore) return;

		auto GUIEngine = AppCore->GetGUIEngine();

		if (GUIEngine)
		{
			if (GUIEngine->IsExistMouseOnGUI()) return;
		}

#ifdef USE_INPUT_SYSTEM
		auto InputState = AppCore->GetInputState();

		InputState->SetWheelScrollAmount(glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset)));
#endif
	}

	bool CDescAppManager::InitWindow()
	{
		glfwInit();

#ifndef USE_OPENGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // OpenGLを使用したくないのでここで明示的に切る
#endif // !USE_OPENGL
		
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Garnet", nullptr, nullptr);

#ifdef USE_OPENGL
		glfwMakeContextCurrent(m_pWindow);
#endif

		glfwSetWindowUserPointer(m_pWindow, this);

		glfwSetKeyCallback(m_pWindow, KetCallback);
		glfwSetFramebufferSizeCallback(m_pWindow, Resize_Callback);
		glfwSetWindowCloseCallback(m_pWindow, Close_Callback);
		glfwSetMouseButtonCallback(m_pWindow, MousebuttonCallback);
		glfwSetCursorPosCallback(m_pWindow, CursorPosCallback);
		glfwSetScrollCallback(m_pWindow, ScrollCallback);

		return true;
	}

	void CDescAppManager::ResizeWindow(int w, int h)
	{
		m_AppCore->Resize(w, h);
	}

	bool CDescAppManager::RunLopp()
	{
		m_IsRunLoop = g_IsRunLoop;

		if (g_IsRunLoop)
		{
			glfwPollEvents();

			if (!Update()) return false;
			if (!LateUpdate()) return false;
			if (!FixedUpdate()) return false;
			if (!Draw()) return false;
			
			m_AppCore->GetInputState()->Clear();
		}
		else
		{
#ifdef __EMSCRIPTEN__
			emscripten_cancel_main_loop();
#endif // __EMSCRIPTEN__
		}

		return true;
	}

	bool CDescAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
#ifdef __EMSCRIPTEN__
		// Web上だとさらに単位が違う
		m_SecondsTime = static_cast<float>(clock()) * 0.001f * 0.001f;
#else
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
#endif
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		if (!m_AppCore->Update(m_SecondsTime, m_DeltaSecondsTime)) return false;

#ifdef _DEBUG
		// FPSの計測と表示(60FPSを基準とする)
		float FPS = 60.0f / (m_DeltaSecondsTime * 60.0f);
		Console::Log("[FPS] %f fps / [CurrentTime] %f s\n", FPS, m_SecondsTime);
#endif // _DEBUG

		return true;
	}

	bool CDescAppManager::LateUpdate()
	{
		if (!m_AppCore->LateUpdate()) return false;

		return true;
	}

	bool CDescAppManager::FixedUpdate()
	{
		if (!m_AppCore->FixedUpdate()) return false;

		return true;
	}

	bool CDescAppManager::Draw()
	{
		if (!m_AppCore->Draw()) return false;

#ifdef USE_OPENGL
		glfwSwapBuffers(m_pWindow);
#endif

		return true;
	}
}
#endif // USE_GLFW