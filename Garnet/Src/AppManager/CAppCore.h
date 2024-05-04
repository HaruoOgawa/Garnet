#pragma once

#include "../Interface/IWindowAPI.h"

#include <memory>

namespace api { class IGraphicsAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }
namespace gui { class IGUIEngine; }

#ifdef USE_WEBGPU
namespace api { class CWebGPUAPI; }
#elif USE_VULKAN
namespace api { class CVulkanAPI; }
#elif USE_OPENGL
namespace api { class COpenGLAPI; }
#endif

namespace app
{
	class CAppCore
	{
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		std::shared_ptr<app::IApp> m_App;

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		std::shared_ptr<gui::IGUIEngine> m_GUIEngine;

#ifdef USE_WEBGPU
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#elif USE_VULKAN
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#elif USE_OPENGL
		std::shared_ptr<api::COpenGLAPI> m_GraphicsAPI;
#endif

		std::shared_ptr<input::CInputState> m_InputState;
	public:
		CAppCore();
		virtual ~CAppCore() = default;

		const std::shared_ptr<gui::IGUIEngine>& GetGUIEngine() const;

		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }

		bool Initialize(IWindowAPI* pWindowAPI);
		bool Release();
		bool Resize(int Width, int Height);
		bool Update(float SecondsTime, float DeltaSecondsTime);
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();
	};
}