#pragma once

#include "IGraphicsAPI.h"
#include <memory>

namespace camera { class CCamera; }
namespace resource { class CLoadWorker; }
namespace graphics { class CDrawInfo; }
namespace input { class CInputState; }

namespace app
{
	class IApp
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) = 0;
		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool Resize(int Width, int Height) = 0;
#ifdef USE_INPUT_SYSTEM
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState) = 0;
#else
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) = 0;
#endif
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) = 0;

		virtual const std::shared_ptr<camera::CCamera>& GetMainCamera() const = 0;

		virtual const std::shared_ptr<graphics::CDrawInfo>& GetDrawInfo() const = 0;
	};
}