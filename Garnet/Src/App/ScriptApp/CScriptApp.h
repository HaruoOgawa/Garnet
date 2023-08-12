#pragma once
#include <memory>

#include "../../Interface/IApp.h"
#include "../../Interface/IGraphicsAPI.h"

namespace scene { class CScriptScene; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }
namespace imageeffect { class CBlurEffect; }
namespace resource { class CLoadWorker; }

namespace app
{
	class CScriptApp : public IApp
	{
		std::shared_ptr<scene::CScriptScene> m_ScriptScene;
		std::shared_ptr<camera::CCamera> m_MainCamera;
		std::shared_ptr<projection::CProjection> m_Projection;
		std::shared_ptr<graphics::CDrawInfo> m_DrawInfo;
		std::shared_ptr<imageeffect::CBlurEffect> m_BlurEffect;
	public:
		CScriptApp();
		virtual ~CScriptApp();

		bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;
		bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Resize(int Width, int Height) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, float SecondsTime) override;

		virtual const std::shared_ptr<camera::CCamera>& GetMainCamera() const override;
	};
}