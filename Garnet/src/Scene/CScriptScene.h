#pragma once
#include <memory>

#include "../Interface/IGraphicsAPI.h"

namespace graphics { class CMesh; }
namespace file { class CFileReader; }

namespace scene
{
	class CScriptScene
	{
		std::shared_ptr<graphics::CMesh> m_TestMesh;
		std::shared_ptr<file::CFileReader> m_VertexShader;
		std::shared_ptr<file::CFileReader> m_FragmentShader;

		bool m_IsLoaded;
	private:
		bool Load(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CScriptScene();
		virtual ~CScriptScene() = default;

		bool Release(api::IGraphicsAPI* pGraphicsAPI);

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) ;
	};
}