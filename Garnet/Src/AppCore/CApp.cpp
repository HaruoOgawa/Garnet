#include "CApp.h"

namespace app
{
	bool CApp::Initialize(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CApp::Release(api::IGraphicsAPI* pGraphicsAPI)
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

	bool CApp::Draw(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<input::CInputState>& InputState, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		return true;
	}


	const std::shared_ptr<graphics::CDrawInfo>& CApp::GetDrawInfo() const
	{
		return nullptr;
	}

	std::vector<unsigned char> CApp::GetEmbeddedBinary(const std::string& Key)
	{
		return {};
	}

	// コンポーネント作成
	std::shared_ptr<scriptable::CComponent> CApp::CreateComponent(const std::string& ComponentType, const std::string& ValueRegistry)
	{
		return nullptr;
	}

	// 起動準備完了
	bool CApp::OnStartup(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		return true;
	}

	// ロード完了イベント
	bool CApp::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<gui::IGUIEngine>& GUIEngine)
	{
		return true;
	}

	// フォーカスイベント
	void CApp::OnFocus(bool Focused, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
	}

	// エラー通知イベント
	void CApp::OnAssertError(const std::string& Message)
	{
	}

	// Getter
	std::vector<std::shared_ptr<object::C3DObject>> CApp::GetObjectList() const
	{
		return std::vector<std::shared_ptr<object::C3DObject>>();
	}

	std::shared_ptr<scene::CSceneController> CApp::GetSceneController() const
	{
		return nullptr;
	}

	// タイムライン再生停止イベント
	void CApp::OnPlayedTimeline(bool IsPlay)
	{
	}
}