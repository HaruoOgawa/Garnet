#include "CAppCore.h"
#include "../Interface/IWindowAPI.h"
#include "../App/ScriptApp/CScriptApp.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../Input/CInputState.h"
#include "../Camera/CCamera.h"
#include "../Graphics/CDrawInfo.h"
#include "../Message/Console.h"

#ifdef USE_GLFW
#include "../WindowAPI/CGLFWWindowAPI.h"
#elif USE_WEB_NATIVE
#include "../WindowAPI/CWebWindowAPI.h"
#elif USE_WIN32_WindowAPI
#include "../WindowAPI/CWin32WindowAPI.h"
#endif // WindowAPI

#ifdef USE_WEBGPU
#include "../GraphicsAPI/WebGPU/CWebGPUAPI.h"
#elif USE_VULKAN
#include "../GraphicsAPI/Vulkan/CVulkanAPI.h"
#elif USE_OPENGL
#include "../GraphicsAPI/OpenGL/COpenGLAPI.h"
#endif // USE_WEBGPU

#ifdef USE_GUIENGINE
#include "../GUIEngine/imgui/CImGuiGUIEngine.h"
#else
#include "../GUIEngine/CDummyGUIEngine.h"
#endif

namespace app
{
	CAppCore::CAppCore(const std::shared_ptr<app::IApp>& App):
		m_WindowAPI(nullptr),
		m_GraphicsAPI(nullptr),
		m_IsRunLoop(true),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
		m_App(App),
		m_InputState(std::make_shared<input::CInputState>()),
		m_LoadWorker(nullptr),
		m_GUIEngine(nullptr)
	{
	}

	CAppCore::~CAppCore()
	{
		Release();
	}

	bool CAppCore::Release()
	{
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

		if (m_GUIEngine)
		{
			m_GUIEngine->Release(m_GraphicsAPI.get());
			m_GUIEngine.reset();
			m_GUIEngine = nullptr;
		}

#ifdef USE_VULKAN
		// 論理デバイスが操作を完了するのを待つ
		vkDeviceWaitIdle(m_GraphicsAPI->GetLogicalDevice());
#endif

		if (m_GraphicsAPI)
		{
			m_GraphicsAPI->Release();
			m_GraphicsAPI.reset();
			m_GraphicsAPI = nullptr;
		}
		
		if (m_WindowAPI)
		{
			m_WindowAPI->Release();
			m_WindowAPI.reset();
			m_WindowAPI = nullptr;
		}

		return true;
	}

	const std::shared_ptr<gui::IGUIEngine>& CAppCore::GetGUIEngine() const
	{
		return m_GUIEngine;
	}

	bool CAppCore::Initialize(int Width, int Height)
	{
		{
			// WindowAPI
#ifdef USE_GLFW
			m_WindowAPI = std::make_shared<window::CGLFWWindowAPI>();
#elif USE_WEB_NATIVE
			m_WindowAPI = std::make_shared<window::CWebWindowAPI>();
#elif USE_WIN32_WindowAPI
			m_WindowAPI = std::make_shared<window::CWin32WindowAPI>();
#endif

			// GraphicsAPI
#ifdef USE_WEBGPU
			m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(Width, Height);
#elif USE_VULKAN
			m_GraphicsAPI = std::make_shared<api::CVulkanAPI>(Width, Height);
#elif USE_OPENGL
			m_GraphicsAPI = std::make_shared<api::COpenGLAPI>(Width, Height);
#endif // USE_WEBGPU
			
			// GUI
#ifdef USE_GUIENGINE
			m_GUIEngine = std::make_shared<gui::CImGuiGUIEngine>();
#else
			m_GUIEngine = std::make_shared<gui::CDummyGUIEngine>();
#endif
		}

		if (!m_WindowAPI->Initialize(this, Width, Height)) return false;

		if (!m_GraphicsAPI->Initialize(m_WindowAPI.get())) return false;

		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(m_GraphicsAPI.get());

#ifdef USE_GUIENGINE
#ifdef USE_GLFW
		if (!m_GUIEngine->InitializeWithGLFW(m_WindowAPI->GetGLFWWindow(), m_GraphicsAPI.get())) return false;
#elif USE_WIN32_WindowAPI
		if (!m_GUIEngine->InitializeWithWin32API(m_WindowAPI->GetWin32Window(), m_GraphicsAPI.get())) return false;
#endif
#endif // USE_GUIENGINE

		if (!m_App->Initialize(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		m_WindowAPI->AssignCurrentWindowSize();

		return true;
	}

	bool CAppCore::Resize(int Width, int Height)
	{
		m_GraphicsAPI->Resize(Width, Height);

		m_App->Resize(Width, Height);

		return true;
	}

	bool CAppCore::RunLoop()
	{
		if (m_IsRunLoop)
		{
			m_WindowAPI->PollEvents();

			if (!Update()) return false;
			if (!LateUpdate()) return false;
			if (!FixedUpdate()) return false;
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

	void CAppCore::SetRunLoop(bool Flag)
	{
		m_IsRunLoop = Flag;
	}

	bool CAppCore::IsRunLoop()
	{ 
		return m_IsRunLoop; 
	}

	bool CAppCore::Update()
	{
		//
		float PrevSecondsTime = m_SecondsTime;
#ifdef __EMSCRIPTEN__
		// Web上だとさらに単位が違う
		m_SecondsTime = static_cast<float>(clock()) * 0.001f * 0.001f;
#else
		m_SecondsTime = static_cast<float>(clock()) * 0.001f;
#endif
		m_DeltaSecondsTime = m_SecondsTime - PrevSecondsTime;

#ifdef _DEBUG
		// FPSの計測と表示(60FPSを基準とする)
		float FPS = 60.0f / (m_DeltaSecondsTime * 60.0f);
		Console::Log("[FPS] %f fps / [CurrentTime] %f s\n", FPS, m_SecondsTime);
#endif // _DEBUG

		//
		m_App->GetDrawInfo()->SetSecondsTime(m_SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(m_DeltaSecondsTime);

		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(m_DeltaSecondsTime, m_InputState);

		if (!m_App->Update(m_GraphicsAPI.get(), m_LoadWorker.get(), m_InputState)) return false;

		return true;
	}

	bool CAppCore::LateUpdate()
	{
		if (!m_App->LateUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::FixedUpdate()
	{
		if (!m_App->FixedUpdate(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::Draw()
	{
		// Prepare
		if (!m_GraphicsAPI->PrepareRender()) return false;

		//  Record Draw Command
		if (!m_App->Draw(m_GraphicsAPI.get(), m_LoadWorker.get(), m_GUIEngine)) return false;

		// Submit
		if (!m_GraphicsAPI->SubmitRender()) return false;

		// SwapBuffer
		m_WindowAPI->SwapWindowBuffers();

		return true;
	}

	// インプットイベント
	void CAppCore::OnKeyDown(std::string key)
	{
		m_WindowAPI->OnKeyDown(key);
	}

	void CAppCore::OnKeyUp(std::string key)
	{
		m_WindowAPI->OnKeyUp(key);
	}

	// リサイズイベント
	void CAppCore::OnResize(int w, int h)
	{
		m_WindowAPI->OnResize(w, h);
	}

	// マウスイベント
	void CAppCore::OnMouseDown(int buttonNum, int x, int y)
	{
		m_WindowAPI->OnMouseDown(buttonNum, x, y);
	}

	void CAppCore::OnMouseUp(int buttonNum, int x, int y)
	{
		m_WindowAPI->OnMouseUp(buttonNum, x, y);
	}

	void CAppCore::OnMouseMove(int x, int y)
	{
		m_WindowAPI->OnMouseMove(x, y);
	}

	void CAppCore::OnMouseWheel(int deltaY)
	{
		m_WindowAPI->OnMouseWheel(deltaY);
	}
}