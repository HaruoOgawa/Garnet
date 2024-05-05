#pragma once

#include "../Interface/IWindowAPI.h"
#include <string>
#include <memory>

namespace api { class IGraphicsAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }
namespace gui { class IGUIEngine; }
namespace window { class IWindowAPI; }

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
		std::shared_ptr<window::IWindowAPI> m_WindowAPI;

#ifdef USE_WEBGPU
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#elif USE_VULKAN
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#elif USE_OPENGL
		std::shared_ptr<api::COpenGLAPI> m_GraphicsAPI;
#endif

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

		std::shared_ptr<app::IApp> m_App;

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		std::shared_ptr<gui::IGUIEngine> m_GUIEngine;

		std::shared_ptr<input::CInputState> m_InputState;
	public:
		CAppCore();
		virtual ~CAppCore() = default;

		const std::shared_ptr<gui::IGUIEngine>& GetGUIEngine() const;

		const std::shared_ptr<input::CInputState>& GetInputState()const { return m_InputState; }

#ifdef USE_WIN32_WindowAPI
		bool Initialize(HINSTANCE hInstance, int Width, int Height);
#else
		bool Initialize(int Width, int Height);
#endif // USE_WIN32_WindowAPI

		bool Release();
		
		bool Resize(int Width, int Height);
		
		bool RunLoop();

		void SetRunLoop(bool Flag);
		bool IsRunLoop();
		
		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();

		// インプットイベント
		void OnKeyDown(std::string key);
		void OnKeyUp(std::string key);

		// リサイズイベント
		void OnResize(int w, int h);

		// マウスイベント
		void OnMouseDown(int buttonNum, int x, int y);
		void OnMouseUp(int buttonNum, int x, int y);
		void OnMouseMove(int x, int y);
		void OnMouseWheel(int deltaY);
	};
}