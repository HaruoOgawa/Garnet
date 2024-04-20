#pragma once

#ifdef USE_GUIENGINE

#include <string>
#include <imgui.h>

namespace gui
{
	class CGraphicsEditingWindow
	{
	public:
		CGraphicsEditingWindow();
		virtual ~CGraphicsEditingWindow();

		bool Draw();
	};
}
#endif // USE_GUIENGINE