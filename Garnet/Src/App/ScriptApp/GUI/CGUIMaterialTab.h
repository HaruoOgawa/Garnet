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
	class CGUIMaterialTab
	{
	private:
		static bool DrawMaterialGUI(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController, 
			int SelectedObjectIndex, int SelectedNodeIndex);

		static float GetFloat(const std::vector<unsigned char>& BufferData, int Offset);
		static int GetInt(const std::vector<unsigned char>& BufferData, int Offset);
	public:
		static bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<object::C3DObject>>& ObjectList, const std::shared_ptr<scene::CSceneController>& SceneController, 
			int SelectedObjectIndex, int SelectedNodeIndex);
	};
}
#endif