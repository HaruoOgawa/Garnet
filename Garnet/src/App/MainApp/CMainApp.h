#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CMainApp : public IApp
	{
	public:
		CMainApp();
		virtual ~CMainApp();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}