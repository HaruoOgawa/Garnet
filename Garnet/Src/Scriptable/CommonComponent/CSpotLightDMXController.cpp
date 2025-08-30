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
		std::string TiltNodeName = std::string(); // Y軸回転
		std::string PanNodeName = std::string();  // X軸回転
		std::string SpotLightFollowTarget = std::string(); // スポットライトが追従するターゲットノード名(例えばエミッタとか)
		std::string SpotLightObjName = std::string();
		std::string SpotLightNodeName = std::string();

		GetValueRegistry()->SetValue("TiltNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, TiltNodeName.c_str(), sizeof(char) * TiltNodeName.size());
		GetValueRegistry()->SetValue("PanNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, PanNodeName.c_str(), sizeof(char) * PanNodeName.size());
		GetValueRegistry()->SetValue("SpotLightFollowTarget", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightFollowTarget.c_str(), sizeof(char) * SpotLightFollowTarget.size());
		GetValueRegistry()->SetValue("SpotLightObjName", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightObjName.c_str(), sizeof(char) * SpotLightObjName.size());
		GetValueRegistry()->SetValue("SpotLightNodeName", graphics::EUniformValueType::VALUE_TYPE_STRING, SpotLightNodeName.c_str(), sizeof(char) * SpotLightNodeName.size());
		GetValueRegistry()->SetValue("UpsideDown", graphics::EUniformValueType::VALUE_TYPE_INT, &glm::ivec1(0)[0], sizeof(int));
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
		if (!m_SpotLightNode) return;

		std::shared_ptr<scriptable::CComponent> SpotLightComponent = nullptr;

		for(const auto& comp : m_SpotLightNode->GetComponentList())
		{
			if (comp->GetComponentName() == "SpotLight")
			{
				SpotLightComponent = comp;
				break;
			}
		}

		if (!SpotLightComponent) return;

		std::string DMXFixtureName = GetValueRegistry()->GetValueString("DMXFixtureName");

		if (Fixture.DeviceName == DMXFixtureName)
		{
			if (Fixture.ChannelNameList.size() != 8) return;

			// Color
			float R = static_cast<float>(DMXData[0]) / 255.0f;
			float G = static_cast<float>(DMXData[1]) / 255.0f;
			float B = static_cast<float>(DMXData[2]) / 255.0f;

			std::vector<float> color = { R, G, B, 1.0f };

			SpotLightComponent->GetValueRegistry()->SetValue("color", graphics::EUniformValueType::VALUE_TYPE_VEC4, &color[0], sizeof(float) * static_cast<int>(color.size()));

			// Dimmer(intensity)
			// 10.0まで明るさが指定できる照明とする
			float intensity = static_cast<float>(DMXData[3]) / 255.0f;
			SpotLightComponent->GetValueRegistry()->SetValue("intensity", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &intensity, sizeof(float));

			// Pan
			float Pan = 2.0f * 3.1415f * static_cast<float>(DMXData[4]) / 255.0f;
			SpotLightComponent->GetValueRegistry()->SetValue("pan", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Pan, sizeof(float));

			// Tilt
			float Tilt = 2.0f * 3.1415f * static_cast<float>(DMXData[5]) / 255.0f;
			SpotLightComponent->GetValueRegistry()->SetValue("tilt", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Tilt, sizeof(float));

			// Angle
			float Angle = 90.0f * static_cast<float>(DMXData[6]) / 255.0f;
			SpotLightComponent->GetValueRegistry()->SetValue("angle", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Angle, sizeof(float));

			// Height
			// 50mまで届くライトとする
			float Height = 50.0f * static_cast<float>(DMXData[7]) / 255.0f;
			SpotLightComponent->GetValueRegistry()->SetValue("height", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Height, sizeof(float));

			// Tilt(Y軸回転)をメッシュに反映
			if (m_TiltNode)
			{
				glm::quat rot = glm::angleAxis(Tilt, glm::vec3(0.0f, 1.0f, 0.0f));
				
				int UpsideDown = GetValueRegistry()->GetValueInt("UpsideDown");
				if (UpsideDown == 1)
				{
					rot = glm::angleAxis(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * rot;
				}

				m_TiltNode->SetRot(rot);
			}

			// Pan(X軸回転)をメッシュに反映
			if (m_PanNode)
			{
				glm::quat rot = glm::angleAxis(Pan, glm::vec3(1.0f, 0.0f, 0.0f));
				m_PanNode->SetRot(rot);
			}
		}
	}
#endif // USE_NETWORK
}