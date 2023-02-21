#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"

namespace scene { class CScriptScene; }

namespace app
{
	class CScriptApp: public IApp
	{
		std::shared_ptr<scene::CScriptScene> m_ScriptScene;
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