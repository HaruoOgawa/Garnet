#ifdef USE_ANIMATION
#include "CIKSolver.h"
#include "CBone.h"

namespace animation
{
	CIKSolver::CIKSolver()
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

		glm::vec3 GoalPos = math::CTransform(m_IKTarget->CalcWorldMatrix()).GetPos();

		// ターゲットに届くかサイクルの最大値に達するまで計算を繰り返す
		int CurrentLoopNum = 0;
		while (CurrentLoopNum < m_IKParam->IKLoopCount)
		{
			bool Result = false;

			//
			auto& EffectorChain = m_IKChainList[Last];
			glm::vec3 EffectorPos = math::CTransform(EffectorChain->CalcWorldMatrix()).GetPos();

			// 既に接触しているなら終了
			if (glm::length(GoalPos - EffectorPos) < Threshold) break;

			for (int j = Size - 2; j >= 0; j--)
			{
				EffectorPos = math::CTransform(EffectorChain->CalcWorldMatrix()).GetPos();

				auto& IKChain = m_IKChainList[j];
				math::CTransform ChainWorldTransform = math::CTransform(IKChain->CalcWorldMatrix());

				glm::vec3 ChainWorldPos = ChainWorldTransform.GetPos();
				glm::quat ChainWorldRot = ChainWorldTransform.GetRot();

				glm::vec3 ToEffector = EffectorPos - ChainWorldPos;
				glm::vec3 ToGoal = GoalPos - ChainWorldPos;

				glm::quat EffectorToGoalQuat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

				if (glm::length(ToGoal) > Epsilon)
				{
					EffectorToGoalQuat = math::CTransform::CalcTwoVectorRotate(ToEffector, ToGoal);
				}

				glm::quat WorldRotated = ChainWorldRot * EffectorToGoalQuat;
				glm::quat LocalRotated = WorldRotated * glm::inverse(ChainWorldRot);

				m_IKChainList[j]->SetRot(LocalRotated * m_IKChainList[j]->GetRot());

				// 接触しているなら終了
				EffectorPos = math::CTransform(EffectorChain->CalcWorldMatrix()).GetPos();

				if (glm::length(GoalPos - EffectorPos) < Threshold)
				{
					Result = true;

					break;
				}
			}

			// ターゲットIKに届いたらループを終了する
			if (Result) break;

			// ループ回数を更新
			CurrentLoopNum++;
		}

		return true;
	}
}
#endif