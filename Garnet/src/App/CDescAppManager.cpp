#include "CDescAppManager.h"
#include "../Debug/Message/Console.h"

#ifdef __DAWN__
#include "../GraphicsAPI/CWebGPUAPI.h"
#else
#include "../GraphicsAPI/CVulkanAPI.h"
#endif // __DAWN__

#include "./ScriptApp/CScriptApp.h"
#include "./EditorApp/CEditorApp.h"
#include "./MainApp/CMainApp.h"

bool g_IsRunLoop = true;

namespace descapp
{
	CDescAppManager::CDescAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_GraphicsAPI(nullptr),
		m_App(nullptr)
	{
		//
#ifdef __DAWN__
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>();
#else
		m_GraphicsAPI = std::make_shared<api::CVulkanAPI>();
#endif // __DAWN__
		
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
	}

	bool CDescAppManager::Release()
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
#ifndef __DAWN__
		if(!m_GraphicsAPI->InitializeWithGLFW(m_pWindow)) return false;
#else
		if (!m_GraphicsAPI->Initialize()) return false;
#endif
		if (!m_App->Initialize(m_GraphicsAPI.get())) return false;

		return true;
	}

	void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			g_IsRunLoop = false;
		}
	}

	void Close_Callback(GLFWwindow* window)
	{
		g_IsRunLoop = false;
	}

	bool CDescAppManager::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // OpenGLを使用したくないのでここで明示的に切る
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Garnet", nullptr, nullptr);

		glfwSetKeyCallback(m_pWindow, Key_Callback);
		glfwSetWindowCloseCallback(m_pWindow, Close_Callback);

		return true;
	}

	bool CDescAppManager::RunLopp()
	{
		while (g_IsRunLoop)
		{
			glfwPollEvents();

			if (!Update()) return false;
			if (!Draw()) return false;
		}

#ifndef __DAWN__
		// 論理デバイスが操作を完了するのを待つ
		vkDeviceWaitIdle(m_GraphicsAPI->GetLogicalDevice());
#endif
		return true;
	}

	bool CDescAppManager::Update()
	{
		if (!m_App->Update(m_GraphicsAPI.get())) return false;

		return true;
	}

	bool CDescAppManager::Draw()
	{
		if (!m_App->Draw(m_GraphicsAPI.get())) return false;

		return true;
	}
}