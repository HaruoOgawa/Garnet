#ifdef USE_GLFW

#include "CGLFWWindowAPI.h"
#include "../Message/Console.h"
#include "../Input/CInputState.h"
#include "../AppCore/CAppCore.h"

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

namespace window
{
	CGLFWWindowAPI::CGLFWWindowAPI():
		m_pCAppCore(nullptr),
		m_pWindow(nullptr)
	{
	}

	GLFWwindow* CGLFWWindowAPI::GetGLFWWindow() const
	{
		return m_pWindow;
	}

	bool CGLFWWindowAPI::Release()
	{
		if (m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();

			m_pWindow = nullptr;
		}

		return true;
	}

	bool CGLFWWindowAPI::Initialize(app::CAppCore* pAppCore, app::SAppSettings Settings)
	{
		m_pCAppCore = pAppCore;

		if (!InitWindow(Settings.ScreenWidth, Settings.ScreenHeight)) return false;

		return true;
	}

	void KetCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
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
			case GLFW_KEY_F1:
				KeyType = input::EKeyType::KEY_TYPE_F1;
				break;
			case GLFW_KEY_LEFT_CONTROL:
				KeyType = input::EKeyType::KEY_TYPE_CONTROL;
				break;
			case GLFW_KEY_SPACE:
				KeyType = input::EKeyType::KEY_TYPE_SPACE;
				break;

			case GLFW_KEY_ENTER:
				KeyType = input::EKeyType::KEY_TYPE_ENTER;
				break;

			default:
				break;
		}

		InputState->SetKeyState(KeyType, (action == GLFW_PRESS || action == GLFW_REPEAT));
#endif

		// ループ終了
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			AppCore->SetRunLoop(false);
		}
	}

	void Resize_Callback(GLFWwindow* window, int width, int height)
	{
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		WindowAPI->ResizeWindow(width, height);
	}

	void Close_Callback(GLFWwindow* window)
	{
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		AppCore->SetRunLoop(false);
	}

	void MousebuttonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
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
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
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
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
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

	void FocusCallback(GLFWwindow* window, int focused)
	{
		auto WindowAPI = reinterpret_cast<CGLFWWindowAPI*>(glfwGetWindowUserPointer(window));
		if (!WindowAPI) return;

		auto AppCore = WindowAPI->GetAppCore();
		if (!AppCore) return;

		AppCore->FocusWindow((focused == 1));
	}

	bool CGLFWWindowAPI::InitWindow(int Width, int Height)
	{
		glfwInit();

#ifndef USE_OPENGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // OpenGLを使用したくないのでここで明示的に切る
#endif // !USE_OPENGL
		
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#ifdef USE_WEBGPU
		std::string Title = "Garnet (WebGPU)";
#elif USE_VULKAN
		std::string Title = "Garnet (Vulkan)";
#elif USE_OPENGL
		std::string Title = "Garnet (OpenGL)";
#endif // USE_WEBGPU

		m_pWindow = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);

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
		glfwSetWindowFocusCallback(m_pWindow, FocusCallback);

		return true;
	}

	void CGLFWWindowAPI::SwapWindowBuffers()
	{
#ifdef USE_OPENGL
		glfwSwapBuffers(m_pWindow);
#endif
	}

	void CGLFWWindowAPI::AssignCurrentWindowSize()
	{
		int w, h;
		glfwGetWindowSize(m_pWindow, &w, &h);

		ResizeWindow(w, h);
	}

	void CGLFWWindowAPI::PollEvents()
	{
		glfwPollEvents();
	}

	app::CAppCore* CGLFWWindowAPI::GetAppCore() const
	{
		return m_pCAppCore;
	}

	void CGLFWWindowAPI::ResizeWindow(int w, int h)
	{
		m_pCAppCore->ResizeWindow(w, h);
	}

	// インプットイベント
	void CGLFWWindowAPI::OnKeyDown(std::string key)
	{
	}

	void CGLFWWindowAPI::OnKeyUp(std::string key)
	{
	}

	// リサイズイベント
	void CGLFWWindowAPI::OnResize(int w, int h)
	{
	}

	// フォーカスイベント
	void CGLFWWindowAPI::OnFocus(int focused)
	{
	}

	// マウスイベント
	void CGLFWWindowAPI::OnMouseDown(int buttonNum, int x, int y)
	{
	}

	void CGLFWWindowAPI::OnMouseUp(int buttonNum, int x, int y)
	{
	}

	void CGLFWWindowAPI::OnMouseMove(int x, int y)
	{
	}

	void CGLFWWindowAPI::OnMouseWheel(int deltaY)
	{
	}
}
#endif // USE_GLFW