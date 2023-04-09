#pragma once
#include <memory>

#include "../Interface/IGraphicsAPI.h"

namespace graphics { class CMesh; }

namespace scene
{
	class CScriptScene
	{
		std::shared_ptr<graphics::CMesh> m_TestMesh;
	public:
		CScriptScene();
		virtual ~CScriptScene() = default;

		bool Release(api::IGraphicsAPI* pGraphicsAPI);

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) ;
	};
}