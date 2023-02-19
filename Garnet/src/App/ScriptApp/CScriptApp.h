#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"

namespace graphics { class CMesh; }

namespace app
{
	class CScriptApp: public IApp
	{
		std::shared_ptr<graphics::CMesh> m_TestMesh;
	public:
		CScriptApp();
		virtual ~CScriptApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}