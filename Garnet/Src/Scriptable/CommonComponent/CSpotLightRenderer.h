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
	class CSpotLightRenderer : public scriptable::CComponent
	{
		resource::ELoadStatus m_Status;

		std::shared_ptr<resource::CMaterialFrameLoader> m_Loader;
		std::shared_ptr<resource::CMaterialFrameLoader> m_SecondLoader;

		std::shared_ptr<object::C3DObject> m_LightObject;
		std::shared_ptr<object::C3DObject> m_LightGeomObject;
		std::vector<std::shared_ptr<graphics::CMaterial>> m_MaterialList;

	private:
		bool CheckIsLoading(bool& Loaded, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
			const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState, const std::shared_ptr<object::C3DObject>& Object);

	public:
		CSpotLightRenderer(const std::string& ComponentName, const std::string& RegistryName);
		virtual ~CSpotLightRenderer();

		virtual bool OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState,
			const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode) override;

		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode) override;
	};
}