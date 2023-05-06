#pragma once
#include <memory>

#include "../Interface/IGraphicsAPI.h"

namespace file { class CFileReader; }
namespace object { class C3DObject; }

namespace scene
{
	class CScriptScene
	{
		std::shared_ptr<object::C3DObject> m_TestObject;

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