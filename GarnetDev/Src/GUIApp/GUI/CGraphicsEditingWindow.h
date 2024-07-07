#pragma once

#ifdef USE_GUIENGINE

#include <string>
#include <memory>
#include <imgui.h>

#include "CGUIObjectTab.h"
#include "CGUIRenderingTab.h"
#include "CGUICameraTab.h"
#include "CGUIResourcesTab.h"
#include "CTimeLineView.h"

#include <Interface/IGUIEngine.h>
#include <Interface/IApp.h>

namespace api { class IGraphicsAPI; }
namespace timeline { class CTimelineController; }

namespace gui
{
	struct SGUIParams
	{
		std::vector<std::shared_ptr<object::C3DObject>> ObjectList;
		std::shared_ptr<scene::CSceneController> SceneController = nullptr;
		std::shared_ptr<app::CFileModifier> FileModifier = nullptr;
		std::shared_ptr<timeline::CTimelineController> TimelineController = nullptr;
	};

	class CGraphicsEditingWindow
	{
		CGUIObjectTab m_GUIObjectTab;
		CGUIResourcesTab m_GUIResourcesTab;
		CTimeLineView m_TimeLineView;
	public:
		CGraphicsEditingWindow();
		virtual ~CGraphicsEditingWindow();

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const SGUIParams& GUIParams,  const std::shared_ptr<gui::IGUIEngine>& GUIEngine);
	};
}
#endif // USE_GUIENGINE