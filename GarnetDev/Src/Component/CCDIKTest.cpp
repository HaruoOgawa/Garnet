#include "CCDIKTest.h"
#include <Object/C3DObject.h>
#include <Scene/CSceneController.h>
#include <Message/Console.h>

namespace component
{
	CCDIKTest::CCDIKTest(const std::string& ComponentName, const std::string& RegistryName):
		CComponent(ComponentName, RegistryName),
		m_Object(nullptr),
		m_NumOfLink(5),
		m_TargetNode(nullptr),
		m_InitTargetPos(glm::vec3(0.0f))
	{
	}

	CCDIKTest::~CCDIKTest()
	{
	}

	bool CCDIKTest::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (!Object) return false;
		m_Object = Object;

		m_TargetNode = Object->FindNodeByName("Target");
		if (!m_TargetNode) return false;

		m_InitTargetPos = m_TargetNode->GetWorldPos();

		// Link0 ~ 4の順番(根本から先端)に入っていて0が根本・4が先端
		// Link数は先端も入れて全部で5
		for (int i = 0; i < m_NumOfLink; i++)
		{
			auto LinkNode = Object->FindNodeByName("Link.00" + std::to_string(i));
			if (!LinkNode) return false;

			m_LinkList.push_back(LinkNode);
		}

		// あれ、直で回転入れても何も反応しない
		glm::quat rot = glm::angleAxis(3.1415f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//m_LinkList[m_NumOfLink - 1]->SetRot(rot);
		// 2個前にすると反応。でも45度とかにするとせん断が発生する
		//m_LinkList[m_NumOfLink - 2]->SetRot(rot);

		return true;
	}

	bool CCDIKTest::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_TargetNode || m_LinkList.empty()) return true;

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

			m_TargetNode->SetPos(Pos);
		}*/

		{
			float r = 2.0f;

			glm::vec3 Pos = m_TargetNode->GetPos();
			Pos.x = r * glm::cos(DrawInfo->GetSecondsTime());
			Pos.y = r * glm::abs(glm::sin(DrawInfo->GetSecondsTime()));

			m_TargetNode->SetPos(Pos);
		}

		const glm::vec3 TargetPos = m_TargetNode->GetWorldPos();

		int NumOfCicle = 64;
		for (int c = 0; c < NumOfCicle; c++)
		{
			std::shared_ptr<object::CNode> EndNode = m_LinkList[m_NumOfLink - 1];
			glm::vec3 EndPos = EndNode->GetWorldPos();

			for (int i = m_NumOfLink - 2; i >= 0; i--)
			{
				std::shared_ptr<object::CNode> LinkNode = m_LinkList[i];

				glm::vec3 LinkPos = LinkNode->GetWorldPos();

				glm::vec3 e_i = glm::normalize(EndPos - LinkPos);
				glm::vec3 t_i = glm::normalize(TargetPos - LinkPos);

				// 内積
				// なぜか1を微妙に越してNaNになってしまうことがあるのでちゃんとクランプしておく
				float dot = glm::clamp(glm::dot(e_i, t_i), -1.0f, 1.0f);

				// 外積
				glm::vec3 axis = glm::cross(e_i, t_i);

				glm::quat rot;
				
				if (glm::length(axis) < 1e-6f)
				{
					if (glm::sign(dot) == 1.0f)
					{
						// 同じ方向に平行な時は回転の必要がない
						continue;
					}
					else
					{
						// 反対方向に平行なので任意の垂直軸で180度回転する
						glm::vec3 XAxis = glm::vec3(1.0f, 0.0f, 0.0f);
						glm::vec3 YAxis = glm::vec3(0.0f, 1.0f, 0.0f);
						
						glm::vec3 SubAxis = glm::cross(XAxis, e_i);

						if (glm::length(SubAxis) < 1e-6f)
						{
							// X軸とも平行なのでY軸の方を使う(さすがにXとYを見れば大丈夫なはず？)
							SubAxis = glm::cross(YAxis, e_i);
						}

						rot = glm::angleAxis(3.1415f, glm::normalize(SubAxis)); // 回転取得前にちゃんと軸を正規化しておく

						if (std::isnan(rot.x) || std::isnan(rot.y) || std::isnan(rot.z) || std::isnan(rot.w))
						{
							float x = 0.0f;
						}
					}
				}
				else
				{
					// 通常通り内積結果から回転
					float angle = glm::acos(dot);
					rot = glm::angleAxis(angle, glm::normalize(axis)); // 回転取得前にちゃんと軸を正規化しておく

					if (std::isnan(rot.x) || std::isnan(rot.y) || std::isnan(rot.z) || std::isnan(rot.w))
					{
						float x = 0.0f;
					}
				}

				if (std::isnan(rot.x) || std::isnan(rot.y) || std::isnan(rot.z) || std::isnan(rot.w))
				{
					float x = 0.0f;
				}

				// 現在のLinkよりも１つ先端に近い方に回転を反映する
				//std::shared_ptr<object::CNode> PrevLinkNode = m_LinkList[i + 1];
				//PrevLinkNode->SetRot(rot * PrevLinkNode->GetRot());

				LinkNode->SetRot(rot * LinkNode->GetRot());

				// ひとまず全てのワールド行列を更新
				m_Object->CalcWorldMatrix();

				// EndNodeの座標を更新
				EndPos = EndNode->GetWorldPos();

				if (glm::distance2(TargetPos, EndPos) < 0.01f)
				{
					// 終了
					return true;
				}
			}
		}

		return true;
	}
}