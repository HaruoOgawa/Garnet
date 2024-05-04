#pragma once

#include <memory>

namespace api { class IGraphicsAPI; }
namespace app { class IApp; }
namespace input { class CInputState; }
namespace resource { class CLoadWorker; }
namespace gui { class IGUIEngine; }

namespace app
{
	class CAppCore
	{
		std::shared_ptr<app::IApp> m_App;
	public:
		CAppCore();
		virtual ~CAppCore();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		bool Release(api::IGraphicsAPI* pGraphicsAPI);
		bool Resize(int Width, int Height);
		bool Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState, float SecondsTime, float DeltaSecondsTime);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine);
	};
}