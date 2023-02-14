#pragma once
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

namespace api { class IGraphicsAPI; }
namespace app{ class IApp; }

namespace descapp
{
	class CDescAppManager
	{
		GLFWwindow* m_pWindow;
		std::shared_ptr<api::IGraphicsAPI> m_GraphicsAPI;
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
	public:
		CDescAppManager(app::EAppType AppType);
		virtual ~CDescAppManager();
		bool Initialize();
		bool RunLopp();

		// インプットイベント
	};
}