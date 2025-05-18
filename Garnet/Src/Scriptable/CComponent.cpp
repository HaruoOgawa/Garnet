#include "CComponent.h"

namespace scriptable
{
	CComponent::CComponent(const std::string& ComponentName, const std::string& RegistryName):
		m_ComponentName(ComponentName),
		m_ValueRegistry(std::make_shared<scriptable::CValueRegistry>(ComponentName, RegistryName))
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
		return m_ValueRegistry->GetRegistryName();
	}

	void CComponent::SetValueRegistry(const std::shared_ptr<scriptable::CValueRegistry>& ValueRegistry)
	{
		auto PrevValueRegistry = m_ValueRegistry; // ˆø‚«Œp‚®
		m_ValueRegistry = ValueRegistry;

		const auto& NewValueList = m_ValueRegistry->GetValueList();
		for (const auto& Value : PrevValueRegistry->GetValueList())
		{
			// PrevValueRegistry‚Ì’l‚ðm_ValueRegistry‚ªŽ‚Á‚Ä‚¢‚È‚¯‚ê‚Î“o˜^‚·‚é
			// m_ValueRegistry‚É‚ÍƒV[ƒ“‚Å‹Lq‚µ‚½’l‚µ‚©“ü‚Á‚Ä‚±‚È‚¢‚½‚ß
			const auto& it = NewValueList.find(Value.first);
			if (it != NewValueList.end()) continue;

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