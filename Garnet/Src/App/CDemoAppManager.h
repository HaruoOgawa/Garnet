#pragma once

#ifdef USE_OPENGL
#include <glfw3.h>
#include <glfw3native.h>

#include <memory>
#include "EAppType.h"

namespace app
{
	class CDemoAppManager
	{
		bool m_IsRunLoop;
	public:
		CDemoAppManager(app::EAppType AppType);
		virtual ~CDemoAppManager();

		bool Initialize();
		bool RunLopp();

		bool IsRunLoop() { return m_IsRunLoop; }

		void ResizeWindow(int w, int h);
	};
}
#endif // USE_OPENGL