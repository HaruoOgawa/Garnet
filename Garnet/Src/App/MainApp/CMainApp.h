#pragma once
#include "../../Interface/IApp.h"
namespace resource { class CLoadWorker; }

namespace app
{
	class CMainApp : public IApp
	{
	public:
		CMainApp();
		virtual ~CMainApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Resize(int Width, int Height) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime) override;

		virtual const std::shared_ptr<camera::CCamera>& GetMainCamera() const override;
	};
}