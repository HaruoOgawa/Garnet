#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <Interface/IGraphicsAPI.h>
#include <Scriptable/CScriptCallback.h>
#include <Scriptable/CValueRegistry.h>

namespace resource { class CLoadWorker; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace input { class CInputState; }
namespace physics { class IPhysicsEngine; }
namespace scene { class CSceneController; }
namespace graphics { class CDrawInfo; }
namespace object { 
	class C3DObject; 
	class CNode;
}

namespace scriptable
{
	class CComponent : public CScriptCallback
	{
		const std::string m_ComponentName;

		std::shared_ptr<scriptable::CValueRegistry> m_ValueRegistry;
	public:
		CComponent(const std::string& ComponentName, const std::string& RegistryName);
		virtual ~CComponent();

		const std::string GetComponentName() const;
		const std::string GetRegistryName() const;

		virtual void SetValueRegistry(const std::shared_ptr<scriptable::CValueRegistry>& ValueRegistry);
		virtual const std::shared_ptr<scriptable::CValueRegistry>& GetValueRegistry() const;

		virtual void Reset();

		virtual bool OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController, 
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode);

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, 
			const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState, 
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode);

		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode);
	};
}