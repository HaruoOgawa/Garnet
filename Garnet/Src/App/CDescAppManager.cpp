#if (defined(USE_VULKAN) || defined(USE_WEBGPU)) && !defined(__EMSCRIPTEN__)

#include "CDescAppManager.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
// emscripten_webgpu_get_deviceの使用に必要なインクルード
#include <emscripten/html5_webgpu.h>
#endif

#ifdef USE_WEBGPU
#include "../GraphicsAPI/WebGPU/CWebGPUAPI.h"
#else
#include "../GraphicsAPI/Vulkan/CVulkanAPI.h"
#endif // USE_WEBGPU

#ifdef USE_VIEWER_CAMERA
#include "../../Camera/CViewerCamera.h"
#endif // USE_VIEWER_CAMERA

#include "../Input/CInputState.h"

#include "./ScriptApp/CScriptApp.h"

bool g_IsRunLoop = true;

namespace descapp
{
	CDescAppManager::CDescAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_GraphicsAPI(nullptr),
		m_App(nullptr),
		m_IsRunLoop(g_IsRunLoop),
		m_SecondsTime(0.0f), 
		m_LoadWorker(nullptr),
#ifdef USE_WEBGPU
		m_InputState(std::make_shared<input::CInputState>(1.0f)),
#else
		m_InputState(std::make_shared<input::CInputState>(1.0f)),
#endif // USE_WEBGPU
		m_DeltaSecondsTime(0.0f)
	{
		//
#ifdef USE_WEBGPU
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(WIDTH, HEIGHT);
#else
		m_GraphicsAPI = std::make_shared<api::CVulkanAPI>(WIDTH, HEIGHT);
#endif // USE_WEBGPU
		
		m_App = std::make_shared<app::CScriptApp>();
	}

	CDescAppManager::~CDescAppManager()
	{
		Release();
	}

	bool CDescAppManager::Release()
	{
#ifdef USE_VULKAN
		// 論理デバイスが操作を完了するのを待つ
		vkDeviceWaitIdle(m_GraphicsAPI->GetLogicalDevice());
#endif

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

		if (m_GraphicsAPI)
		{
			m_GraphicsAPI->Release();
			m_GraphicsAPI.reset();
			m_GraphicsAPI = nullptr;
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
#ifdef __EMSCRIPTEN__
		if (!m_GraphicsAPI->Initialize()) return false;
#else
		if(!m_GraphicsAPI->InitializeWithGLFW(m_pWindow)) return false;
#endif
		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(m_GraphicsAPI.get());

		if (!m_App->Initialize(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		int w, h;
		glfwGetWindowSize(m_pWindow, &w, &h);

		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);

		return true;
	}

	void KetCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		//
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		auto InputState = AppManager->GetInputState();

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
			default:
				break;
		}

		InputState->SetKeyState(KeyType, (action == GLFW_PRESS || action == GLFW_REPEAT));

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
		auto InputState = AppManager->GetInputState();

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
	}

	void CursorPosCallback(GLFWwindow* window, double PosX, double PosY)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		auto InputState = AppManager->GetInputState();

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
	}

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto AppManager = reinterpret_cast<CDescAppManager*>(glfwGetWindowUserPointer(window));
		auto InputState = AppManager->GetInputState();

		InputState->SetWheelScrollAmount(glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset)));
	}

	bool CDescAppManager::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // OpenGLを使用したくないのでここで明示的に切る
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Garnet", nullptr, nullptr);

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
		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);
	}

	bool CDescAppManager::RunLopp()
	{
		m_IsRunLoop = g_IsRunLoop;

		if (g_IsRunLoop)
		{
			glfwPollEvents();

			if (!Update()) return false;
			if (!Draw()) return false;

			m_InputState->Clear();
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
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		m_App->GetDrawInfo()->SetSecondsTime(m_SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(m_DeltaSecondsTime);

		// ViewCameraのUpdate
#ifdef USE_INPUT_SYSTEM
		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_DeltaSecondsTime, m_InputState);
#endif // USE_INPUT_SYSTEM

		if (!m_App->Update(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

#ifdef _DEBUG
		// FPSの計測と表示(60FPSを基準とする)
		float FPS = 60.0f / (m_DeltaSecondsTime * 60.0f);
		//Console::Log("[FPS] %f fps / [CurrentTime] %f s\n", FPS, m_SecondsTime);
#endif // _DEBUG

		return true;
	}

	bool CDescAppManager::Draw()
	{
		if (!m_App->Draw(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}
}
#endif