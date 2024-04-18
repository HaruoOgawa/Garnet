#pragma once

#ifdef USE_GUIENGINE

#include <imgui.h>

namespace gui
{
	class CGraphicsEditingWindow
	{
		float m_TestFloatVal;
		float m_TestFloatSliderVal;
	public:
		CGraphicsEditingWindow();
		virtual ~CGraphicsEditingWindow();

		bool Draw();
	};
}
#endif // USE_GUIENGINE