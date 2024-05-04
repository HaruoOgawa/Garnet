#pragma once

#include "../Interface/IWindowAPI.h"

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

		std::shared_ptr<resource::CLoadWorker> m_LoadWorker;

		std::shared_ptr<gui::IGUIEngine> m_GUIEngine;
	public:
		CAppCore();
		virtual ~CAppCore() = default;

		const std::shared_ptr<gui::IGUIEngine>& GetGUIEngine() const;

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, IWindowAPI* pWindowAPI);
		bool Release(api::IGraphicsAPI* pGraphicsAPI);
		bool Resize(int Width, int Height);
		bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<input::CInputState>& InputState, float SecondsTime, float DeltaSecondsTime);
		bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI);
		bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI);
	};
}