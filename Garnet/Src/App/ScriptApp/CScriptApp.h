#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"

namespace scene { class CScriptScene; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }
namespace imageeffect { class CBlurEffect; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace object { class C3DObject; }

namespace gui
{ 
	class IGUIEngine; 
	class CGraphicsEditingWindow; 
}
namespace physics { class IPhysicsEngine; }

namespace app
{
	class CScriptApp : public IApp
	{
		std::shared_ptr<physics::IPhysicsEngine> m_PhysicsEngine;

		std::shared_ptr<scene::CScriptScene> m_ScriptScene;
		std::shared_ptr<camera::CCamera> m_MainCamera;
		std::shared_ptr<projection::CProjection> m_Projection;
		std::shared_ptr<graphics::CDrawInfo> m_DrawInfo;
		std::shared_ptr<imageeffect::CBlurEffect> m_BlurEffect;

		std::shared_ptr<gui::CGraphicsEditingWindow> m_GraphicsEditingWindow;
		
	public:
		CScriptApp();
		virtual ~CScriptApp();

		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		virtual bool Resize(int Width, int Height) override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState) override;
		virtual bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;
		virtual bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;

		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine) override;

		virtual const std::shared_ptr<camera::CCamera>& GetMainCamera() const override;

		virtual const std::shared_ptr<graphics::CDrawInfo>& GetDrawInfo() const override;

		virtual const std::vector<std::shared_ptr<object::C3DObject>>& GetObjectList() const override;
	};
}