#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IPhysicsEngine.h"

namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }
namespace animation { class CAnimationClipSet; }
namespace graphics 
{ 
	class CDrawInfo; 
	class CTexture;
	class CMaterialFrame;
}


namespace scene
{
	class CScriptScene
	{
		bool m_IsLoaded;

		std::shared_ptr<graphics::CMaterialFrame> m_BasicToonMF;
		std::shared_ptr<graphics::CMaterialFrame> m_SimpleTextureMF;
		std::shared_ptr<graphics::CMaterialFrame> m_DepthMF;
		std::shared_ptr<graphics::CMaterialFrame> m_PBRMF;
		std::shared_ptr<graphics::CMaterialFrame> m_PBRMorph_MF;

		//
		std::shared_ptr<graphics::CTexture> m_Cube_Texture;

		// glTF
		std::shared_ptr<object::C3DObject> m_SimpleMorphObj;
		std::shared_ptr<object::C3DObject> m_BrainStemObj;

		// Fbx
		std::shared_ptr<object::C3DObject> m_Walk_Animation;
		std::shared_ptr<object::C3DObject> m_Jump_Animation;
		std::shared_ptr<object::C3DObject> m_Punch_Animation;
		std::shared_ptr<object::C3DObject> m_XBotObject;
		std::shared_ptr<object::C3DObject> m_MouseyObject;
		std::shared_ptr<object::C3DObject> m_WarrokObject;

		// PhysicsObj
		std::shared_ptr<object::C3DObject> m_PhysicsGround;
		std::shared_ptr<object::C3DObject> m_PhysicsSphere;
		std::shared_ptr<object::C3DObject> m_PhysicsCubeList;

		// MMD
		std::shared_ptr<object::C3DObject> m_TdaMiku_Model;
		std::shared_ptr<animation::CAnimationClipSet> m_VMDAnimationSet;
		std::shared_ptr<animation::CAnimationClipSet> m_ExpressionVMD;

		// Background
		std::shared_ptr<graphics::CTexture> m_IBL_Skybox_Texture;
		std::shared_ptr<object::C3DObject> m_Background;
		std::shared_ptr<object::C3DObject> m_DebugSphere;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker);

		float rand(const glm::vec2& st) { return static_cast<float>(glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898, 78.233))) * 43758.5453123)); }
	public:
		CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		virtual ~CScriptScene();

		bool IsLoaded() const;

#ifdef USE_INPUT_SYSTEM
		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
#else
		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
#endif

		bool Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) ;

		// Tex of FrameBuffer
		void SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
	};
}