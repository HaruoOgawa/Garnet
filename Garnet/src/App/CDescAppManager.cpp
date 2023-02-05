#include "CDescAppManager.h"
#include "../Debug/Message/Console.h"

#include "../GraphicsAPI/CVulkanAPI.h"
#include "./ScriptApp/CScriptApp.h"
#include "./EditorApp/CEditorApp.h"
#include "./MainApp/CMainApp.h"

bool g_IsRunLoop = true;

namespace descapp
{
	CDescAppManager::CDescAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_pGraphicsAPI(nullptr),
		m_App(nullptr)
	{
		Console::Log("CDescAppManager::CDescAppManager\n");

		//
		m_pGraphicsAPI = std::make_shared<api::CVulkanAPI>();

		//
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