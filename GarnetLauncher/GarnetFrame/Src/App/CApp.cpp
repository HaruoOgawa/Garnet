#include "CApp.h"

namespace app
{
	CApp::CApp()
	{
	}

	CApp::~CApp()
	{
	}

	bool CApp::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CApp::Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CApp::ProcessInput(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CApp::Resize(int Width, int Height)
	{
		return true;
	}

	bool CApp::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState)
	{
		return true;
	}

	bool CApp::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CApp::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CApp::Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		if (!pGraphicsAPI->BeginRender()) return false;

		if (!pGraphicsAPI->EndRender()) return false;

		return true;
	}

	const std::shared_ptr<graphics::CDrawInfo>& CApp::GetDrawInfo() const
	{
		return nullptr;
	}

	std::vector<std::shared_ptr<object::C3DObject>> CApp::GetObjectList() const
	{
		return std::vector<std::shared_ptr<object::C3DObject>>();
	}

	// フォーカスイベント
	void CApp::OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
	}
}