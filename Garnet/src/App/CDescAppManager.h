#pragma once

#ifndef __Dawn__
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // !__Dawn__

#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

#ifdef __Dawn__
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
#ifdef __Dawn__
		std::shared_ptr<api::CWebGPUAPI> m_GraphicsAPI;
#else
		std::shared_ptr<api::CVulkanAPI> m_GraphicsAPI;
#endif
		std::shared_ptr<app::IApp> m_App;

		const unsigned int WIDTH = 800;
		const unsigned int HEIGHT = 600;

		// インプットプールを作成する
		// 1フレームに1つしか受け取れなかったり、途中で処理を挟まれたりするのもよくないので、1フレームの間のインプットをプールに溜めて
		// まとめて実行する

		// 使用するGraphicsAPIのクラスもここでインスタンス化する
		// Desctopなら

		// アプリケーションタイプはメインからもらう
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

		// インプットイベント
	};
}