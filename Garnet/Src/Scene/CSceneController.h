#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

#include "../../../Interface/IGraphicsAPI.h"
#include "../../../Interface/IPhysicsEngine.h"

namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace graphics { 
	class CDrawInfo; 
	class CMaterialFrame;
}

namespace scene
{
	class CSceneController
	{
		std::vector<std::shared_ptr<object::C3DObject>> m_ObjectList;

		std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>> m_MaterialFrameMap;
	public:
		CSceneController();
		virtual ~CSceneController();

		void AddObject(const std::shared_ptr<object::C3DObject>& Object);
		std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const;

		void AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame);

		bool Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}