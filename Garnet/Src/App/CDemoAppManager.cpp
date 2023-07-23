#ifdef USE_OPENGL

#include "CDemoAppManager.h"

namespace app
{
	CDemoAppManager::CDemoAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_IsRunLoop(true)
	{
	}

	CDemoAppManager::~CDemoAppManager()
	{
		if (m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();

			m_pWindow = nullptr;
		}
	}

	bool CDemoAppManager::Initialize()
	{
		if (!InitWindow()) return false;

		return true;
	}

	void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			AppManager->SetRunLoop(false);
		}
	}

	void Resize_Callback(GLFWwindow* window, int width, int height)
	{
		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
		AppManager->ResizeWindow(width, height);
	}

	void Close_Callback(GLFWwindow* window)
	{
		auto AppManager = reinterpret_cast<CDemoAppManager*>(glfwGetWindowUserPointer(window));
		AppManager->SetRunLoop(false);
	}

	bool CDemoAppManager::RunLopp()
	{
		return true;
	}

	void CDemoAppManager::ResizeWindow(int w, int h)
	{
	}

	bool CDemoAppManager::InitWindow()
	{
		glfwInit();
		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Garnet", nullptr, nullptr);

		glfwSetWindowUserPointer(m_pWindow, this);

		glfwSetKeyCallback(m_pWindow, Key_Callback);
		glfwSetFramebufferSizeCallback(m_pWindow, Resize_Callback);
		glfwSetWindowCloseCallback(m_pWindow, Close_Callback);

		return true;
	}
}
#endif // USE_OPENGL