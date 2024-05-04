#pragma once

#include "../Interface/IWindowAPI.h"

#ifdef USE_GLFW
#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

namespace app{ class CAppCore; }

namespace descapp
{
	class CDescAppManager : public IWindowAPI
	{
		GLFWwindow* m_pWindow;

		std::shared_ptr<app::CAppCore> m_AppCore;
		
		const int WIDTH = 1920;
		const int HEIGHT = 1080;

		bool m_IsRunLoop;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

	private:
		bool InitWindow();

		bool Release();

		bool Update();
		bool LateUpdate();
		bool FixedUpdate();
		bool Draw();
	public:
		CDescAppManager(app::EAppType AppType);
		virtual ~CDescAppManager();

		virtual GLFWwindow* GetGLFWWindow() const override;

		const std::shared_ptr<app::CAppCore>& GetAppCore() const;
		
		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }
		
		void ResizeWindow(int w, int h);
	};
}
#endif