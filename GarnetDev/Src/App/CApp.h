#pragma once

#include <Interface/IApp.h>

namespace app
{
	class CApp : public IApp
	{
	public:
		CApp();
		virtual ~CApp();

		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) override;
		virtual bool Resize(int Width, int Height) override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState) override;
		virtual bool LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;
		virtual bool FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;

		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine) override;

		virtual const std::shared_ptr<graphics::CDrawInfo>& GetDrawInfo() const override;

		virtual std::vector<std::shared_ptr<object::C3DObject>> GetObjectList() const override;

		// フォーカスイベント
		virtual void OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;
	};
}