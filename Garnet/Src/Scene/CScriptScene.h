#pragma once
#include <memory>
#include <vector>

#include "../Interface/IGraphicsAPI.h"

namespace file { class CFile; }
namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { 
	class CDrawInfo; 
	class CTexture;
}
namespace resource { class CLoadWorker; }

namespace scene
{
	class CScriptScene
	{
		bool m_IsLoaded;

		// Tex of FrameBuffer
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;

		// DepthMaterial(LoadWorkerÇ∆MaterialFrameé¿ëïÇ‹Ç≈ÇÕÇ–Ç∆Ç‹Ç∏SceneÇ…èëÇ¢ÇƒÇ®Ç≠ Å® å„ÅXC3DObjectÇ…à⁄çsÇ∑ÇÈ)
		std::shared_ptr<file::CFile> m_DepthVertex;
		std::shared_ptr<file::CFile> m_DepthFragment;

		// Test Obj
		std::shared_ptr<object::C3DObject> m_TestObject;

		std::shared_ptr<file::CFile> m_VertexShader;
		std::shared_ptr<file::CFile> m_FragmentShader;

		std::shared_ptr<file::CFile> m_Texture0;
		std::shared_ptr<file::CFile> m_Texture1;

		// glTF
		std::shared_ptr<object::C3DObject> m_Sphere_glTFObj;
		std::shared_ptr<file::CFile> m_Sphere_glTFData;
		std::shared_ptr<object::C3DObject> m_Helmet_glTFObj;
		std::shared_ptr<file::CFile> m_Helmet_glTFData;
		std::shared_ptr<file::CFile> m_glTFVert;
		std::shared_ptr<file::CFile> m_glTFFrag;

		// Cubemap
		std::shared_ptr<file::CFile> m_Cube0;
		std::shared_ptr<file::CFile> m_Cube1;
		std::shared_ptr<file::CFile> m_Cube2;
		std::shared_ptr<file::CFile> m_Cube3;
		std::shared_ptr<file::CFile> m_Cube4;
		std::shared_ptr<file::CFile> m_Cube5;

		// ShadowMapping
		std::shared_ptr<object::C3DObject> m_DepthDebugObj;
		std::shared_ptr<file::CFile> m_ShadowDebugVertex;
		std::shared_ptr<file::CFile> m_ShadowDebugFragment;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		virtual ~CScriptScene();

		bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) ;

		// Tex of FrameBuffer
		void SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
	};
}