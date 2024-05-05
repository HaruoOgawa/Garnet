#pragma once

#include "IGraphicsAPI.h"
#include <memory>

namespace camera { class CCamera; }
namespace resource { class CLoadWorker; }
namespace graphics { class CDrawInfo; }
namespace input { class CInputState; }
namespace gui { class IGUIEngine; }
namespace object { class C3DObject; }
namespace physics { class IPhysicsEngine; }

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

		virtual const std::vector<std::shared_ptr<object::C3DObject>>& GetObjectList() const = 0;
	};
}