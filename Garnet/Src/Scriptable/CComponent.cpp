#include "CComponent.h"

namespace scriptable
{
	CComponent::CComponent(const std::string& Name):
		CValueRegistry(Name),
		m_Name(Name)
	{
	}

	CComponent::~CComponent()
	{
	}

	const std::string CComponent::GetName() const
	{
		return m_Name;
	}

	void CComponent::OnLoaded(const std::shared_ptr<scene::CSceneController>& SceneController)
	{
	}

	bool CComponent::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CComponent::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		return true;
	}

	bool CComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}
}