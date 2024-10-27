#include "CCDIKTest.h"
#include <Object/C3DObject.h>
#include <Scene/CSceneController.h>
#include <Message/Console.h>

namespace component
{
	CCDIKTest::CCDIKTest(const std::string& ComponentName, const std::string& RegistryName):
		CComponent(ComponentName, RegistryName),
		m_Target(nullptr),
		m_InitTargetPos(glm::vec3(0.0f))
	{
	}

	CCDIKTest::~CCDIKTest()
	{
	}

	bool CCDIKTest::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		m_Target = Object->FindNodeByName("Target");
		if (!m_Target) return false;

		m_InitTargetPos = m_Target->GetWorldPos();

		for (int i = 4; i >= 0; i--)
		{
			auto LinkNode = Object->FindNodeByName("Link.00" + std::to_string(i));
			if (!LinkNode) return false;

			m_LinkList.push_back(LinkNode);
		}

		return true;
	}

	bool CCDIKTest::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_Target || m_LinkList.empty()) return true;

		//
		

		// マウス位置にターゲットを追従させる
		/*if (InputState->IsKeyDown(input::EKeyType::KEY_TYPE_1))
		{
			glm::vec2 Res = Projection->GetScreenResolution();

			// マウスのスクリーン座標を計算
			glm::vec2 MousePos = InputState->GetMousePos();
			//MousePos.x *= Res.y / Res.x;
			glm::vec4 MouseScreenPos = glm::vec4(MousePos.x, -MousePos.y, 0.0f, 1.0f); // カメラにぴったりくっついているものとして考えるのでZを0.0fとしている
			
			glm::vec4 MouseViewPos = glm::inverse(Projection->GetPrejectionMatrix()) * MouseScreenPos;
			
			// 射影除算を行う
			Console::Log("[Prev] MouseViewPos => x: %f, y: %f, z: %f, w: %f\n", MouseViewPos.x, MouseViewPos.y, MouseViewPos.z, MouseViewPos.w);

			MouseViewPos /= MouseViewPos.w;

			Console::Log("[After] MouseViewPos => x: %f, y: %f, z: %f, w: %f\n", MouseViewPos.x, MouseViewPos.y, MouseViewPos.z, MouseViewPos.w);

			glm::vec4 MouseWorldPos = glm::inverse(Camera->GetViewMatrix()) * MouseViewPos;

			Console::Log("[After] MouseWorldPos => x: %f, y: %f, z: %f, w: %f\n", MouseWorldPos.x, MouseWorldPos.y, MouseWorldPos.z, MouseWorldPos.w);

			glm::vec3 Pos = glm::vec3(MouseWorldPos.x, MouseWorldPos.y, MouseWorldPos.z);

			// カメラとターゲットの初期値の
			Pos += (m_InitTargetPos - Camera->GetPos());

			Console::Log("[After] Pos => x: %f, y: %f, z: %f\n", Pos.x, Pos.y, Pos.z);

			m_Target->SetPos(Pos);
		}*/

		return true;
	}

	bool CCDIKTest::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (!m_Target || m_LinkList.empty()) return true;

		return true;
	}
}