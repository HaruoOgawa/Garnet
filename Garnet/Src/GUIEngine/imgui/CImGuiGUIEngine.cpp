#ifdef USE_GUIENGINE

#include "CImGuiGUIEngine.h"
#include "../../Message/Console.h"

namespace gui
{
	CImGuiGUIEngine::CImGuiGUIEngine():
		m_Initialized(false),
		m_ImGuiCore(nullptr)
	{
#ifdef USE_VULKAN
		m_ImGuiCore = std::make_shared<CImGuiCoreVulkan>();
#elif USE_WEBGPU
		m_ImGuiCore = std::make_shared<CImGuiCoreWebGPU>();
#elif USE_OPENGL
		m_ImGuiCore = std::make_shared<CImGuiCoreOpenGL>();
#endif 
	}

	CImGuiGUIEngine::~CImGuiGUIEngine()
	{
	}

	void CImGuiGUIEngine::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_ImGuiCore)
		{
			m_ImGuiCore->Release(pGraphicsAPI);
			m_ImGuiCore.reset();
			m_ImGuiCore = nullptr;
		}

#ifdef USE_GLFW
		ImGui_ImplGlfw_Shutdown();
#elif USE_WIN32_WindowAPI
		ImGui_ImplWin32_Shutdown();
#endif // USE_GLFW
		
		ImGui::DestroyContext();
	}

	bool CImGuiGUIEngine::Initialize(window::IWindowAPI* pWindowAPI, api::IGraphicsAPI* pGraphicsAPI)
	{
		// Dear ImGuiのコンテキストを作成
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard

		// Window APIの初期化
#ifdef USE_VULKAN
		ImGui_ImplGlfw_InitForVulkan(pWindowAPI->GetGLFWWindow(), true);
#elif USE_WEBGPU
		ImGui_ImplGlfw_InitForOther(pWindowAPI->GetGLFWWindow(), true);
#elif defined(USE_OPENGL) && defined(USE_GLFW)
		ImGui_ImplGlfw_InitForOpenGL(pWindowAPI->GetGLFWWindow(), true);
#elif defined(USE_OPENGL) && defined(USE_WIN32_WindowAPI)
		ImGui_ImplWin32_InitForOpenGL(pWindowAPI->GetWin32Window());
#endif

#ifdef __EMSCRIPTEN__
		ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#MainCanvas");
#endif // __EMSCRIPTEN__

		// GraphicsAPI周りの初期化
		if (!m_ImGuiCore->Initialize(pGraphicsAPI)) return false;

		m_Initialized = true;

		return true;
	}

#ifdef USE_WIN32_WindowAPI
	bool CImGuiGUIEngine::CheckInput(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		return ImGui_ImplWin32_WndProcHandler(window, msg, w_param, l_param);
	}
#endif

	bool CImGuiGUIEngine::BeginFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_ImGuiCore->BeginFrame(pGraphicsAPI)) return false;

#ifdef USE_GLFW
		ImGui_ImplGlfw_NewFrame();
#elif USE_WIN32_WindowAPI
		ImGui_ImplWin32_NewFrame();
#endif

		ImGui::NewFrame();

		return true;
	}

	bool CImGuiGUIEngine::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui::Render();

		if (!m_ImGuiCore->EndFrame(pGraphicsAPI)) return false;

		return true;
	}

	bool CImGuiGUIEngine::IsExistMouseOnGUI()
	{
		if (!m_Initialized) return false;

		ImGuiIO& io = ImGui::GetIO();

		return io.WantCaptureMouse;
	}
}
#endif // USE_GUIENGINE