#ifdef USE_OPENGL

#include "CDemoAppManager.h"
#include "../GraphicsAPI/OpenGL/COpenGLAPI.h"
#include "./ScriptApp/CScriptApp.h"
#include "./EditorApp/CEditorApp.h"
#include "./MainApp/CMainApp.h"

namespace app
{
	CDemoAppManager::CDemoAppManager(app::EAppType AppType):
		m_pWindow(nullptr),
		m_IsRunLoop(true),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
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
		if (!m_GraphicsAPI->InitializeWithGLFW(m_pWindow)) return false;

		if (!m_App->Initialize(m_GraphicsAPI.get())) return false;

		int w, h;
		glfwGetWindowSize(m_pWindow, &w, &h);

		m_GraphicsAPI->Resize(w, h);
		m_App->Resize(w, h);

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
		if (m_IsRunLoop)
		{
			glfwPollEvents();

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

	bool CDemoAppManager::Update()
	{
		float PrevSecondsTime = m_SecondsTime;
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

		if (!m_App->Update(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		return true;
	}

	bool CDemoAppManager::Draw()
	{
		if (!m_App->Draw(m_GraphicsAPI.get(), m_SecondsTime)) return false;

		return true;
	}
}
#endif // USE_OPENGL