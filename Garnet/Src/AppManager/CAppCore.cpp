#include "CAppCore.h"
#include "../Interface/IWindowAPI.h"
#include "../App/ScriptApp/CScriptApp.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../Input/CInputState.h"
#include "../Camera/CCamera.h"
#include "../Graphics/CDrawInfo.h"

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
	CAppCore::CAppCore():
		m_GraphicsAPI(nullptr),
		m_App(nullptr),
		m_InputState(std::make_shared<input::CInputState>()),
		m_LoadWorker(nullptr),
		m_GUIEngine(nullptr)
	{
#ifdef USE_WEBGPU
		m_GraphicsAPI = std::make_shared<api::CWebGPUAPI>(WIDTH, HEIGHT);
#elif USE_VULKAN
		m_GraphicsAPI = std::make_shared<api::CVulkanAPI>(WIDTH, HEIGHT);
#elif USE_OPENGL
		m_GraphicsAPI = std::make_shared<api::COpenGLAPI>(WIDTH, HEIGHT);
#endif // USE_WEBGPU

		m_App = std::make_shared<app::CScriptApp>();

#ifdef USE_GUIENGINE
		m_GUIEngine = std::make_shared<gui::CImGuiGUIEngine>();
#else
		m_GUIEngine = std::make_shared<gui::CDummyGUIEngine>();
#endif
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

		return true;
	}

	const std::shared_ptr<gui::IGUIEngine>& CAppCore::GetGUIEngine() const
	{
		return m_GUIEngine;
	}

	bool CAppCore::Initialize(IWindowAPI* pWindowAPI)
	{
		if (!m_GraphicsAPI->Initialize(pWindowAPI)) return false;

		// ロードワーカー
		m_LoadWorker = std::make_shared<resource::CLoadWorker>(m_GraphicsAPI.get());

#ifdef USE_GUIENGINE
#ifdef USE_GLFW
		if (!m_GUIEngine->InitializeWithGLFW(pWindowAPI->GetGLFWWindow(), m_GraphicsAPI.get())) return false;
#elif USE_WIN32_WindowAPI
		if (!m_GUIEngine->InitializeWithWin32API(pWindowAPI->GetWin32Window(), m_GraphicsAPI.get())) return false;
#endif
#endif // USE_GUIENGINE

		//
		if (!m_App->Initialize(m_GraphicsAPI.get(), m_LoadWorker.get())) return false;

		return true;
	}

	bool CAppCore::Resize(int Width, int Height)
	{
		m_GraphicsAPI->Resize(Width, Height);

		m_App->Resize(Width, Height);

		return true;
	}

	bool CAppCore::Update(float SecondsTime, float DeltaSecondsTime)
	{
		m_App->GetDrawInfo()->SetSecondsTime(SecondsTime);
		m_App->GetDrawInfo()->SetDeltaSecondsTime(DeltaSecondsTime);

		const auto& MainCamera = m_App->GetMainCamera();
		if (MainCamera) MainCamera->Update(DeltaSecondsTime, m_InputState);

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

		return true;
	}
}