#pragma once

#ifdef USE_GUIENGINE
#include <imgui.h>
#include <string>
#include <vector>
#include <memory>

namespace object {
	class C3DObject;
	class CNode;
}

namespace gui
{
	class CGUIMaterialTab
	{
	public:
		static bool Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, int SelectedObjectIndex, int SelectedNodeIndex);
	};
}
#endif