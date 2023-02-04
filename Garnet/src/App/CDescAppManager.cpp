#include "CDescAppManager.h"
#include "../Debug/Message/Console.h"

bool g_IsRunLoop = true;

namespace descapp
{
	CDescAppManager::CDescAppManager():
		m_pWindow(nullptr)
	{
		Console::Log("CDescAppManager::CDescAppManager\n");
	}

	CDescAppManager::~CDescAppManager()
	{
		Release();

		Console::Log("CDescAppManager::~CDescAppManager\n");
	}

	bool CDescAppManager::Release()
	{
		Console::Log("CDescAppManager::Release\n");

		if (m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();
		}

		return true;
	}

	bool CDescAppManager::Initialize()
	{
		Console::Log("CDescAppManager::Initialize\n");

		InitWindow();

		return true;
	}

	void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			g_IsRunLoop = false;
		}
	}

	bool CDescAppManager::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // OpenGLÇégópÇµÇΩÇ≠Ç»Ç¢ÇÃÇ≈Ç±Ç±Ç≈ñæé¶ìIÇ…êÿÇÈ
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Garnet", nullptr, nullptr);

		glfwSetKeyCallback(m_pWindow, Key_Callback);

		return true;
	}

	bool CDescAppManager::RunLopp()
	{
		while (g_IsRunLoop)
		{
			//Console::Log("CDescAppManager::RunLopp\n");

			glfwPollEvents();
		}

		return true;
	}
}