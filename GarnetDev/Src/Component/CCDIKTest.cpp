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

		return true;
	}

	bool CCDIKTest::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_TargetNode || m_LinkList.empty()) return true;
		
		// ターゲットのアニメーション
		{
			float r = 2.0f;

			glm::vec3 Pos = m_TargetNode->GetPos();
			Pos.x = r * glm::cos(DrawInfo->GetSecondsTime());
			Pos.y = r * glm::abs(glm::sin(DrawInfo->GetSecondsTime()));
			Pos.z = r * glm::sin(DrawInfo->GetSecondsTime());

			m_TargetNode->SetPos(Pos);
		}

		// 注意点
		// * せん断が発生してしまうのでCCDIKのリンクのスケールは必ず(1, 1, 1)になるようにする
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

						rot = glm::angleAxis(3.1415f, glm::normalize(SubAxis)); // 回転角度がおかしくなってしまうので回転取得前にちゃんと軸を正規化しておく
					}
				}
				else
				{
					// 通常通り内積結果から回転
					float angle = glm::acos(dot);
					rot = glm::angleAxis(angle, glm::normalize(axis)); // 回転角度がおかしくなってしまうので回転取得前にちゃんと軸を正規化しておく
				}

				if (std::isnan(rot.x) || std::isnan(rot.y) || std::isnan(rot.z) || std::isnan(rot.w))
				{
					Console::Log("[Error] CCDIK - found NaN value in ik rot.\n");
					return false;
				}

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