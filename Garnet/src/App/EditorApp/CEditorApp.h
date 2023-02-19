#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CEditorApp : public IApp
	{
	public:
		CEditorApp();
		virtual ~CEditorApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}