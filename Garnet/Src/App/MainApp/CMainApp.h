#pragma once
#include "../../Interface/IApp.h"

namespace app
{
	class CMainApp : public IApp
	{
	public:
		CMainApp();
		virtual ~CMainApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Resize(int Width, int Height) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}