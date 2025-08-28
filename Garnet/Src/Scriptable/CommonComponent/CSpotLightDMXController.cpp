#include "CSpotLightDMXController.h"
#include "../../Scene/CSceneController.h"
#include "../../Object/C3DObject.h"

namespace scriptable
{
	CSpotLightDMXController::CSpotLightDMXController(const std::string& ComponentName, const std::string& RegistryName):
		CComponent(ComponentName, RegistryName),
		m_PanNode(nullptr),
		m_TiltNode(nullptr),
		m_SpotLightFollowTargetNode(nullptr),
		m_SpotLightObj(nullptr),
		m_SpotLightNode(nullptr)

	{
		std::string PanNodeName = std::string();
		std::string TiltNodeName = std::string();
		std::string SpotLightFollowTarget = std::string(); // スポットライトが追従するターゲットノード名(例えばエミッタとか)
		std::string SpotLightObjName = std::string();
		std::string SpotLightNodeName = std::string();

		GetValueRegistry()->SetValue("PanNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, PanNodeName.c_str(), sizeof(char) * PanNodeName.size());
		GetValueRegistry()->SetValue("TiltNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, TiltNodeName.c_str(), sizeof(char) * TiltNodeName.size());
		GetValueRegistry()->SetValue("SpotLightFollowTarget", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightFollowTarget.c_str(), sizeof(char) * SpotLightFollowTarget.size());
		GetValueRegistry()->SetValue("SpotLightObjName", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightObjName.c_str(), sizeof(char) * SpotLightObjName.size());
		GetValueRegistry()->SetValue("SpotLightNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightNodeName.c_str(), sizeof(char) * SpotLightNodeName.size());
	}

	CSpotLightDMXController::~CSpotLightDMXController()
	{
	}

	bool CSpotLightDMXController::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		std::string PanNodeName = GetValueRegistry()->GetValueString("PanNodeName");
		std::string TiltNodeName = GetValueRegistry()->GetValueString("TiltNodeName");
		std::string SpotLightFollowTarget = GetValueRegistry()->GetValueString("SpotLightFollowTarget");
		std::string SpotLightObjName = GetValueRegistry()->GetValueString("SpotLightObjName");
		std::string SpotLightNodeName = GetValueRegistry()->GetValueString("SpotLightNodeName");

		m_PanNode = Object->FindNodeByName(PanNodeName);
		m_TiltNode = Object->FindNodeByName(TiltNodeName);
		m_SpotLightFollowTargetNode = Object->FindNodeByName(SpotLightFollowTarget); // 追従ターゲットノード

		//
		m_SpotLightObj = SceneController->FindObjectByName(SpotLightObjName);

		if (m_SpotLightObj)
		{
			m_SpotLightNode = m_SpotLightObj->FindNodeByName(SpotLightNodeName);
		}

		return true;
	}

	bool CSpotLightDMXController::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CSpotLightDMXController::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (!m_SpotLightNode || !m_SpotLightFollowTargetNode || !m_SpotLightObj) return true;

		//
		const auto& Scale = Object->GetScale();
		const auto& Rotate = Object->GetRot();
		auto WorldPos = m_SpotLightFollowTargetNode->GetWorldPos();

		WorldPos *= Scale;

		glm::vec4 RotPos = glm::vec4(WorldPos, 1.0f);
		RotPos = glm::mat4_cast(Rotate) * RotPos;
		WorldPos = glm::vec3(RotPos.x, RotPos.y, RotPos.z);

		m_SpotLightNode->SetPos(WorldPos);

		return true;
	}

#ifdef USE_NETWORK
	void CSpotLightDMXController::OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData)
	{
	}
#endif // USE_NETWORK
}