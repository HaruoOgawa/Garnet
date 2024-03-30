#ifdef USE_ANIMATION
#include "CIKSolver.h"
#include "CBone.h"

#include "../Debug/Message/Console.h"

namespace animation
{
	CIKSolver::CIKSolver():
		m_IKParam(nullptr),
		m_IKTarget(nullptr),
		m_OriginWorldMatrix(glm::mat4(1.0f))
	{
	}

	CIKSolver::~CIKSolver()
	{
	}

	bool CIKSolver::Create(const std::shared_ptr<CBone>& IKTargetBone, const std::vector<std::shared_ptr<CBone>>& BoneList)
	{
		m_IKTarget = IKTargetBone->GetBoneNode();
		m_IKParam = IKTargetBone->GetIKParam();

		// ChainListを作成
		for (int i = static_cast<int>(m_IKParam->IKLinkList.size()) - 1; i >= 0; i--)
		{
			const auto& IKLink = m_IKParam->IKLinkList[i];

			int BoneIndex = IKLink.IKLinkBoneIndex;
			if (BoneIndex < 0 || BoneIndex >= BoneList.size()) return false;

			m_IKChainList.push_back(BoneList[BoneIndex]->GetBoneNode());
		}

		// EndEffectorをChainの末尾に追加
		int EndEffectorIndex = m_IKParam->IKTargetBoneIndex;
		if (EndEffectorIndex < 0 || EndEffectorIndex >= BoneList.size()) return false;

		m_IKChainList.push_back(BoneList[EndEffectorIndex]->GetBoneNode());

		return true;
	}

	bool CIKSolver::Solve()
	{
		// CCD-IKを採用
		int Size = static_cast<int>(m_IKChainList.size());

		// CCD-IKには最低でも２つChainが必要
		if (Size < 2) return true;

		int Last = Size - 1;

		float Threshold = 0.01f;

		constexpr float Epsilon = std::numeric_limits<float>::epsilon();

		// Chainの始点となる共通の親ワールド行列
		m_OriginWorldMatrix = m_IKChainList[0]->GetParentNode()->CalcWorldMatrix();

		//
		glm::vec3 GoalPos = math::CTransform(m_IKTarget->CalcWorldMatrix()).GetPos();

		// ターゲットに届くかサイクルの最大値に達するまで計算を繰り返す
		int CurrentLoopNum = 0;
		//const int MaxLoopNum = m_IKParam->IKLoopCount;
		// 重いので最大30回にする
		const int MaxLoopNum = std::min(30, m_IKParam->IKLoopCount);

		while (CurrentLoopNum < MaxLoopNum)
		{
			bool Result = false;

			glm::vec3 EffectorPos = GetWorldTransform(Last).GetPos();

			// 既に接触しているなら終了
			if (glm::length(GoalPos - EffectorPos) < Threshold) break;

			for (int j = Size - 2; j >= 0; j--)
			{
				EffectorPos = GetWorldTransform(Last).GetPos();

				math::CTransform ChainWorldTransform = GetWorldTransform(j);

				glm::vec3 ChainWorldPos = ChainWorldTransform.GetPos();
				glm::quat ChainWorldRot = ChainWorldTransform.GetRot();

				glm::vec3 ToEffector = EffectorPos - ChainWorldPos;
				glm::vec3 ToGoal = GoalPos - ChainWorldPos;

				glm::quat EffectorToGoalQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

				float Angle = 3.1415f;

				if (glm::length(ToGoal) > Epsilon)
				{
					EffectorToGoalQuat = math::CTransform::CalcTwoVectorRotate(glm::normalize(ToEffector), glm::normalize(ToGoal), Angle, m_IKParam->LimitedAngle);
					//EffectorToGoalQuat = math::CTransform::CalcTwoVectorRotate(glm::normalize(ToEffector), glm::normalize(ToGoal), Angle);
				}

				glm::quat WorldRotated = ChainWorldRot * EffectorToGoalQuat;

				// 角度制限を行うかどうか
				{
					int LinkIndex = static_cast<int>(m_IKParam->IKLinkList.size()) - 1 - j;
					if (m_IKParam->IKLinkList[LinkIndex].IsLimitAngle)
					{
						math::CTransform::ClampRotate(WorldRotated, m_IKParam->IKLinkList[LinkIndex].LowerAngle, m_IKParam->IKLinkList[LinkIndex].UpperAngle);
					}
				}

				glm::quat LocalRotated = WorldRotated * glm::inverse(ChainWorldRot);

				m_IKChainList[j]->SetRot(LocalRotated * m_IKChainList[j]->GetRot());

				// 接触しているなら終了
				EffectorPos = GetWorldTransform(Last).GetPos();

				if (glm::length(GoalPos - EffectorPos) < Threshold)
				{
					Result = true;

					break;
				}

				// 接触していなくてかつ回転角度がほぼ0の時は、ToEffectorとToGoalが平行で永遠に動かなくなってしまうのでわざと少しだけ回転してあげる
				// 回転角度は調整(もしかしたらこれがPmxでいう単位角なのかも？)
				if (Angle < 0.001f)
				{
					//m_IKChainList[j]->SetRot(glm::angleAxis(3.1415f / 6.0f, glm::vec3(0.0f, 0.0f, 1.0f)) * m_IKChainList[j]->GetRot());
					//m_IKChainList[j]->SetRot(glm::angleAxis(m_IKParam->LimitedAngle, glm::vec3(0.0f, 0.0f, 1.0f)) * m_IKChainList[j]->GetRot());
				}
			}

			// ターゲットIKに届いたらループを終了する
			if (Result) break;

			// ループ回数を更新
			CurrentLoopNum++;
		}

		return true;
	}

	math::CTransform CIKSolver::GetWorldTransform(int ChainIndex)
	{
		glm::mat4 WorldMatrix = m_OriginWorldMatrix;

		for (int i = 0; i <= ChainIndex; i++)
		{
			WorldMatrix *= m_IKChainList[i]->GetLocalMatrix();
		}

		math::CTransform WorldTransform = math::CTransform(WorldMatrix);

		return WorldTransform;
	}
}
#endif