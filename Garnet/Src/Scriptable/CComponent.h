#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <Interface/IGraphicsAPI.h>
#include <Scriptable/CValueRegistry.h>

namespace resource { class CLoadWorker; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace input { class CInputState; }
namespace physics { class IPhysicsEngine; }
namespace scene { class CSceneController; }
namespace graphics { class CDrawInfo; }

namespace scriptable
{
	class CComponent : public CValueRegistry
	{
		const std::string m_Name;
	public:
		CComponent(const std::string& Name);
		virtual ~CComponent();

		const std::string GetName() const;

		virtual void OnLoaded(const std::shared_ptr<scene::CSceneController>& SceneController);

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);

		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}