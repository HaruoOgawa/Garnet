#pragma once

#include <Scriptable/CComponent.h>
#include <LoadWorker/ELoadStatus.h>

namespace object {
	class C3DObject;
	class CNode;
}
namespace resource { class CMaterialFrameLoader; }
namespace graphics { class CMaterial; }

namespace scriptable
{
	class CSpotLightDMXController : public scriptable::CComponent
	{
		std::shared_ptr<object::CNode> m_PanNode;
		std::shared_ptr<object::CNode> m_TiltNode;
		std::shared_ptr<object::CNode> m_SpotLightFollowTargetNode;

		std::shared_ptr<object::C3DObject> m_SpotLightObj;
		std::shared_ptr<object::CNode> m_SpotLightNode;

	public:
		CSpotLightDMXController(const std::string& ComponentName, const std::string& RegistryName);
		virtual ~CSpotLightDMXController();

		virtual bool OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState,
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode) override;

#ifdef USE_NETWORK
		virtual void OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData) override;
#endif // USE_NETWORK
	};
}