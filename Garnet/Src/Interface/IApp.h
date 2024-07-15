#pragma once

#include "IGraphicsAPI.h"
#include <memory>
#include <vector>

namespace physics { class IPhysicsEngine; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }
namespace imageeffect { class CBlurEffect; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace object { class C3DObject; }
namespace scene { class CSceneController; }
namespace gui { class IGUIEngine; }

namespace app
{
	class IApp
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) = 0;
		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool Resize(int Width, int Height) = 0;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState) = 0;
		virtual bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) = 0;
		virtual bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) = 0;
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine) = 0;

		virtual const std::shared_ptr<graphics::CDrawInfo>& GetDrawInfo() const = 0;

		// ロード完了イベント
		virtual bool OnLoaded(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine) = 0;

		// フォーカスイベント
		virtual void OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) = 0;
	};
}