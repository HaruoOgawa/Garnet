#pragma once
#include <memory>

#include "../Interface/IGraphicsAPI.h"

namespace file { class CFileReader; }
namespace object { class C3DObject; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }

namespace scene
{
	class CScriptScene
	{
		//
		std::shared_ptr<object::C3DObject> m_TestObject;

		std::shared_ptr<file::CFileReader> m_VertexShader;
		std::shared_ptr<file::CFileReader> m_FragmentShader;

		std::shared_ptr<file::CFileReader> m_Texture0;
		std::shared_ptr<file::CFileReader> m_Texture1;

		//
		std::shared_ptr<object::C3DObject> m_glTFObj;
		std::shared_ptr<file::CFileReader> m_glTFData;
		std::shared_ptr<file::CFileReader> m_glTFVert;
		std::shared_ptr<file::CFileReader> m_glTFFrag;

		bool m_IsLoaded;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CScriptScene();
		virtual ~CScriptScene() = default;

		bool Release(api::IGraphicsAPI* pGraphicsAPI);

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) ;
	};
}