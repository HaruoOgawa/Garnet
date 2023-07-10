#pragma once
#include <memory>
#include <vector>

#include "../Interface/IGraphicsAPI.h"

namespace file { class CFileReader; }
namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { 
	class CDrawInfo; 
	class CTexture;
}

namespace scene
{
	class CScriptScene
	{
		// Tex of FrameBuffer
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;

		// DepthMaterial(LoadWorkerÇ∆MaterialFrameé¿ëïÇ‹Ç≈ÇÕÇ–Ç∆Ç‹Ç∏SceneÇ…èëÇ¢ÇƒÇ®Ç≠ Å® å„ÅXC3DObjectÇ…à⁄çsÇ∑ÇÈ)
		std::shared_ptr<file::CFileReader> m_DepthVertex;
		std::shared_ptr<file::CFileReader> m_DepthFragment;

		// Test Obj
		std::shared_ptr<object::C3DObject> m_TestObject;

		std::shared_ptr<file::CFileReader> m_VertexShader;
		std::shared_ptr<file::CFileReader> m_FragmentShader;

		std::shared_ptr<file::CFileReader> m_Texture0;
		std::shared_ptr<file::CFileReader> m_Texture1;

		// glTF
		std::shared_ptr<object::C3DObject> m_Sphere_glTFObj;
		std::shared_ptr<file::CFileReader> m_Sphere_glTFData;
		std::shared_ptr<object::C3DObject> m_Helmet_glTFObj;
		std::shared_ptr<file::CFileReader> m_Helmet_glTFData;
		std::shared_ptr<file::CFileReader> m_glTFVert;
		std::shared_ptr<file::CFileReader> m_glTFFrag;

		// Cubemap
		std::shared_ptr<file::CFileReader> m_Cube0;
		std::shared_ptr<file::CFileReader> m_Cube1;
		std::shared_ptr<file::CFileReader> m_Cube2;
		std::shared_ptr<file::CFileReader> m_Cube3;
		std::shared_ptr<file::CFileReader> m_Cube4;
		std::shared_ptr<file::CFileReader> m_Cube5;

		// ShadowMapping
		std::shared_ptr<object::C3DObject> m_DepthDebugObj;
		std::shared_ptr<file::CFileReader> m_ShadowVertex;
		std::shared_ptr<file::CFileReader> m_ShadowFragment;

		//
		bool m_IsLoaded;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CScriptScene();
		virtual ~CScriptScene() = default;

		bool Release(api::IGraphicsAPI* pGraphicsAPI);

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		bool Update(api::IGraphicsAPI* pGraphicsAPI);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) ;
		bool DrawDebugObj(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		// Tex of FrameBuffer
		void SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture);
	};
}