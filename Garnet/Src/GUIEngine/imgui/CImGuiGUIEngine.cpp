#ifdef USE_GUIENGINE

#include "CImGuiGUIEngine.h"
#include "../../Message/Console.h"

#include <imgui.h>
#ifdef USE_GLFW
#include <imgui_impl_glfw.h>
#elif USE_WIN32_WindowAPI
#include <imgui_impl_win32.h>
#endif // USE_GLFW

#ifdef USE_VULKAN
#include "Core/CImGuiCoreVulkan.h"
#elif USE_WEBGPU
#include "Core/CImGuiCoreWebGPU.h"
#elif USE_OPENGL
#include "Core/CImGuiCoreOpenGL.h"
#endif 

namespace gui
{
	CImGuiGUIEngine::CImGuiGUIEngine():
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

#ifdef USE_GLFW
	bool CImGuiGUIEngine::InitializeWithGLFW(GLFWwindow* pWindow, api::IGraphicsAPI* pGraphicsAPI)
	{
		// Dear ImGuiのコンテキストを作成
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard

		// Window APIの初期化
#ifdef USE_VULKAN
		ImGui_ImplGlfw_InitForVulkan(pWindow, true);
#elif USE_WEBGPU
		ImGui_ImplGlfw_InitForOther(pWindow, true);
#endif

#ifdef __EMSCRIPTEN__
		ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#MainCanvas");
#endif // __EMSCRIPTEN__

		// GraphicsAPI周りの初期化
		if (!m_ImGuiCore->Initialize(pGraphicsAPI)) return false;

		return true;
	}
#elif USE_WIN32_WindowAPI
	bool CImGuiGUIEngine::InitializeWithWin32API(HWND window, api::IGraphicsAPI* pGraphicsAPI)
	{
		// Dear ImGuiのコンテキストを作成
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard

		// Window APIの初期化
#ifdef USE_OPENGL
		ImGui_ImplWin32_InitForOpenGL(window);
#endif // USE_OPENGL

		// GraphicsAPI周りの初期化
		if (!m_ImGuiCore->Initialize(pGraphicsAPI)) return false;

		return true;
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

		ImGui::Begin("Hello World");

		ImGui::Text("This is Hello World Text");

		static float f = 0.0f;
		static int counter = 0;

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();

		return true;
	}

	bool CImGuiGUIEngine::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui::Render();

		if (!m_ImGuiCore->EndFrame(pGraphicsAPI)) return false;

		return true;
	}
}
#endif // USE_GUIENGINE