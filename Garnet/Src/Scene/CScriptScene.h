#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "../Interface/IGraphicsAPI.h"

namespace resource { class CFile; }
namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { 
	class CDrawInfo; 
	class CTexture;
	class CMaterialFrame;
}
namespace resource { class CLoadWorker; }
namespace input { class CInputState; }

namespace scene
{
	class CScriptScene
	{
		bool m_IsLoaded;

		std::shared_ptr<graphics::CMaterialFrame> m_SampleMF;
		std::shared_ptr<object::C3DObject> m_MfTestObject;

		// Fbx
		std::shared_ptr<resource::CFile> m_FbxAnimationData;
		std::shared_ptr<resource::CFile> m_FbxData;
		std::shared_ptr<object::C3DObject> m_FbxObject;

		// Tex of FrameBuffer
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;

		// DepthMaterial(LoadWorkerÇ∆MaterialFrameé¿ëïÇ‹Ç≈ÇÕÇ–Ç∆Ç‹Ç∏SceneÇ…èëÇ¢ÇƒÇ®Ç≠ Å® å„ÅXC3DObjectÇ…à⁄çsÇ∑ÇÈ)
		std::shared_ptr<resource::CFile> m_DepthVertex;
		std::shared_ptr<resource::CFile> m_DepthFragment;

		// IBL
		std::shared_ptr<resource::CFile> m_IBL_Skybox;
		std::shared_ptr<resource::CFile> m_IBL_DiffuseEnvMap;
		std::shared_ptr<resource::CFile> m_IBL_SpecularEnvMap;
		std::shared_ptr<resource::CFile> m_IBL_GGX_LUT;

		// glTF
		std::shared_ptr<resource::CFile> m_glTFData;
		std::shared_ptr<object::C3DObject> m_glTFObject;
		
		std::shared_ptr<resource::CFile> m_BrainStemData;
		std::shared_ptr<object::C3DObject> m_BrainStemDObject;
		
		std::shared_ptr<resource::CFile> m_VRMData;
		std::shared_ptr<object::C3DObject> m_VRMObject;

		// Object
		std::shared_ptr<object::C3DObject> m_Background;
		std::shared_ptr<object::C3DObject> m_DebugSphere;

		// Shader
		std::shared_ptr<resource::CFile> m_VertexShader;
		std::shared_ptr<resource::CFile> m_FragmentShader;
		std::shared_ptr<resource::CFile> m_MinimumVert;
		std::shared_ptr<resource::CFile> m_TextureFrag;

		// Cubemap
		std::shared_ptr<resource::CFile> m_Cube0;
		std::shared_ptr<resource::CFile> m_Cube1;
		std::shared_ptr<resource::CFile> m_Cube2;
		std::shared_ptr<resource::CFile> m_Cube3;
		std::shared_ptr<resource::CFile> m_Cube4;
		std::shared_ptr<resource::CFile> m_Cube5;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);

		float rand(const glm::vec2& st) { return static_cast<float>(glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898, 78.233))) * 43758.5453123)); }
	public:
		CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		virtual ~CScriptScene();

		bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);
		
		bool Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) ;

		// Tex of FrameBuffer
		void SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
	};
}