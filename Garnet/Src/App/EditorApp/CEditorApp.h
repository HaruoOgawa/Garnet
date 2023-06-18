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
		bool Resize(int Width, int Height) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual const std::shared_ptr<camera::CCamera>& GetMainCamera() const override;
	};
}