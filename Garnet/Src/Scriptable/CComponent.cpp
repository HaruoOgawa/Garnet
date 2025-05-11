#include "CComponent.h"

namespace scriptable
{
	CComponent::CComponent(const std::string& ComponentName, const std::string& RegistryName):
		m_ComponentName(ComponentName),
		m_RegistryName(RegistryName),
		m_ValueRegistry(std::make_shared<scriptable::CValueRegistry>(RegistryName))
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
		auto PrevValueRegistry = m_ValueRegistry; // ˆø‚«Œp‚®
		m_ValueRegistry = ValueRegistry;

		for (const auto& Value : PrevValueRegistry->GetValueList())
		{
			m_ValueRegistry->SetValue(Value.second.Name, Value.second.Type, &Value.second.Buffer[0], Value.second.ByteSize);
		}
	}

	const std::shared_ptr<scriptable::CValueRegistry>& CComponent::GetValueRegistry() const
	{
		return m_ValueRegistry;
	}

	void CComponent::Reset()
	{
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
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState, 
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}

	bool CComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}
}