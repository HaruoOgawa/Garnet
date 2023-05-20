#pragma once

#ifndef __DAWN__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // !__DAWN__

#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

#ifdef __DAWN__
namespace api { class CWebGPUAPI; }
#else
namespace api { class CVulkanAPI; }
#endif
namespace app{ class IApp; }

namespace descapp
{
	class CDescAppManager
	{
		GLFWwindow* m_pWindow;
#ifdef __DAWN__
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#else
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#endif
		std::shared_ptr<app::IApp> m_App;

		const unsigned int WIDTH = 800;
		const unsigned int HEIGHT = 600;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

	private:
		bool InitWindow();

		bool Release();

		bool Update();
		bool Draw();
	public:
		CDescAppManager(app::EAppType AppType);
		virtual ~CDescAppManager();
		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }

		// インプットイベント
	};
}