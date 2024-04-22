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
	class CGUIObjectTab
	{
		int m_OperateButtonID;

		int m_SelectedObjectIndex;
		int m_SelectedNodeIndex;
	private:
		void Reset();

		bool DrawNodeGUI(int& SelectedObjectIndex, int& SelectedNodeIndex, int CurrentObjectIndex, int CurrentNodeIndex, 
			const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		void SetDrawable(bool Flag, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

	public:
		CGUIObjectTab();
		virtual ~CGUIObjectTab() = default;

		bool Draw(const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList);
	};
}
#endif