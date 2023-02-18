#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Interface/IRenderer.h"

namespace app
{
	class CScriptApp: public IApp
	{
		std::shared_ptr<renderer::IRenderer> m_TestRenderer;
	public:
		CScriptApp();
		virtual ~CScriptApp();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}