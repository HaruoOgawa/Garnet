#pragma once

#ifdef USE_GUIENGINE
#include <imgui.h>
#include <string>
#include <vector>
#include <memory>

namespace api { class IGraphicsAPI; }
namespace scene { class CSceneController; }

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

		std::string m_SelectedName;
	private:
		void Reset();

		bool DrawNodeGUI(int& SelectedObjectIndex, int& SelectedNodeIndex, int CurrentObjectIndex, int CurrentNodeIndex, 
			const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

		void SetDrawable(bool Flag, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList);

	public:
		CGUIObjectTab();
		virtual ~CGUIObjectTab() = default;

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController);
	};
}
#endif