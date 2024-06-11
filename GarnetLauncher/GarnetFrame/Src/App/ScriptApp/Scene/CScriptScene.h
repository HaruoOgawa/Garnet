#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include <LoadWorker/CLoadWorker.h>
#include <Scene/CSceneController.h>

namespace app
{
	class CScriptScene
	{
		bool m_IsLoaded;

		std::shared_ptr<scene::CSceneController> m_SceneController;

		std::vector<std::shared_ptr<object::C3DObject>> m_ObjectList;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker);

		float rand(const glm::vec2& st) { return static_cast<float>(glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898, 78.233))) * 43758.5453123)); }
	public:
		CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, physics::IPhysicsEngine* pPhysicsEngine);
		virtual ~CScriptScene();

		bool IsLoaded() const;

		const std::shared_ptr<scene::CSceneController>& GetSceneController() const;
		std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const;

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}