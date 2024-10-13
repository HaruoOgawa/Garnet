#include "CComponent.h"

namespace scriptable
{
	CComponent::CComponent(const std::string& ComponentName, const std::string& RegistryName):
		m_ComponentName(ComponentName),
		m_RegistryName(RegistryName),
		m_ValueRegistry(nullptr)
	{
	}

	CComponent::~CComponent()
	{
	}

	const std::string CComponent::GetComponentName() const
	{
		return m_ComponentName;
	}

	const std::string CComponent::GetRegistryName() const
	{
		return m_RegistryName;
	}

	void CComponent::SetValueRegistry(const std::shared_ptr<scriptable::CValueRegistry>& ValueRegistry)
	{
		m_ValueRegistry = ValueRegistry;
	}

	const std::shared_ptr<scriptable::CValueRegistry>& CComponent::GetValueRegistry() const
	{
		return m_ValueRegistry;
	}

	bool CComponent::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
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

	bool CComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}
}