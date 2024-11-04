#ifdef USE_ANIMATION
#include "CIKSolver.h"
#include "CBone.h"

#include "../Message/Console.h"

namespace animation
{
	CIKSolver::CIKSolver():
		m_IKParam(nullptr),
		m_IKTarget(nullptr)
	{
	}

	CIKSolver::~CIKSolver()
	{
	}

	bool CIKSolver::Create(const std::shared_ptr<CBone>& IKTargetBone, const std::vector<std::tuple<std::string, std::shared_ptr<CBone>>>& BoneList)
	{
		m_IKTarget = IKTargetBone->GetBoneNode();
		m_IKParam = IKTargetBone->GetIKParam();

		// ChainListを作成
		for (int i = static_cast<int>(m_IKParam->IKLinkList.size()) - 1; i >= 0; i--)
		{
			const auto& IKLink = m_IKParam->IKLinkList[i];

			int BoneIndex = IKLink.IKLinkBoneIndex;
			if (BoneIndex < 0 || BoneIndex >= BoneList.size()) return false;

			m_IKChainList.push_back(std::get<1>(BoneList[BoneIndex])->GetBoneNode());
		}

		// EndEffectorをChainの末尾に追加
		int EndEffectorIndex = m_IKParam->IKTargetBoneIndex;
		if (EndEffectorIndex < 0 || EndEffectorIndex >= BoneList.size()) return false;

		// 根本から先端の方向でLinkNodeが入っている
		m_IKChainList.push_back(std::get<1>(BoneList[EndEffectorIndex])->GetBoneNode());

		return true;
	}

	bool CIKSolver::Solve()
	{
		// CCD-IKを採用
		const int NumOfLink = static_cast<int>(m_IKChainList.size());

		const int EndIndex = NumOfLink - 1;

		float Threshold = 0.01f;

		const glm::vec3 TargetPos = m_IKTarget->GetWorldPos();

		// ターゲットに届くかサイクルの最大値に達するまで計算を繰り返す
		int CurrentLoopNum = 0;
		bool DoLoop = true;

		const int MaxLoopNum = m_IKParam->IKLoopCount;

		std::shared_ptr<object::CNode> EndNode = m_IKChainList[EndIndex];

		while (DoLoop && CurrentLoopNum < MaxLoopNum)
		{
			glm::vec3 EndPos = EndNode->GetWorldPos();

			// 既に接触しているなら終了
			if (glm::distance2(TargetPos, EndPos) < Threshold)
			{
				DoLoop = false;
				break;
			}

			for (int i = NumOfLink - 2; i >= 0; i--)
			{
				std::shared_ptr<object::CNode> LinkNode = m_IKChainList[i];

				glm::vec3 LinkPos = LinkNode->GetWorldPos();

				glm::vec3 e_i = glm::normalize(EndPos - LinkPos);
				glm::vec3 t_i = glm::normalize(TargetPos - LinkPos);

				// 内積
				// なぜか1を微妙に越してNaNになってしまうことがあるのでちゃんとクランプしておく
				float dot = glm::clamp(glm::dot(e_i, t_i), -1.0f, 1.0f);

				// 外積
				glm::vec3 axis = glm::cross(e_i, t_i);

				glm::quat rot;

				// EffectVecとTargetVecがほぼ平行なので回転軸が存在しない(ほぼ0)になっていることがあるのでそれを考慮する
				// ほぼ平行の時は任意な垂直時軸に対して0度か180度回転させる
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
						glm::vec3 ZAxis = glm::vec3(0.0f, 0.0f, 1.0f);

						glm::vec3 SubAxis = glm::cross(XAxis, e_i);

						if (glm::length(SubAxis) < 1e-6f)
						{
							// X軸とも平行なのでY軸の方を使う
							SubAxis = glm::cross(YAxis, e_i);

							if (glm::length(SubAxis) < 1e-6f)
							{
								// Y軸とも平行なのでZ軸の方を使う
								SubAxis = glm::cross(ZAxis, e_i);
							}
						}

						rot = glm::angleAxis(3.1415f, glm::normalize(SubAxis)); // 回転角度がおかしくなってしまうので回転取得前にちゃんと軸を正規化しておく
					}
				}
				else
				{
					// 通常通り内積結果から回転
					float angle = glm::acos(dot);

					// 単位角で回転量を制限。LimitedAngleはラジアン
					angle = glm::min(angle, m_IKParam->LimitedAngle);

					rot = glm::angleAxis(angle, glm::normalize(axis)); // 回転角度がおかしくなってしまうので回転取得前にちゃんと軸を正規化しておく
				}

				// 回転角度制限
				// 制限を行うことで例えば膝が変な方向に曲がらないようにする
				int LinkIndex = static_cast<int>(m_IKParam->IKLinkList.size()) - 1 - i;
				const auto& IKLink = m_IKParam->IKLinkList[LinkIndex];

				if (IKLink.IsLimitAngle)
				{
					const auto& LowerAngle = IKLink.LowerAngle;
					const auto& UpperAngle = IKLink.UpperAngle;

					glm::vec3 euler = glm::eulerAngles(rot);

					// オイラー角に対して角度制限を行う
					// LowerAngleとUpperAngleはラジアン
					euler.x = glm::clamp(euler.x, LowerAngle.x, UpperAngle.x);
					euler.y = glm::clamp(euler.y, LowerAngle.y, UpperAngle.y);
					euler.z = glm::clamp(euler.z, LowerAngle.z, UpperAngle.z);

					rot = glm::quat(euler);
				}

				LinkNode->SetRot(rot * LinkNode->GetRot());

				if (std::isnan(rot.x) || std::isnan(rot.y) || std::isnan(rot.z) || std::isnan(rot.w))
				{
					Console::Log("[Error] CCDIK - found NaN value in ik rot. when clamp rotation.\n");
					return false;
				}

				// Linkノードのワールド行列を再計算する
				for (int n = i; n < NumOfLink; n++)
				{
					std::shared_ptr<object::CNode> ReCalcNode = m_IKChainList[n];

					const auto& ParentNode = ReCalcNode->GetParentNode();
					if (!ParentNode)
					{
						// 親ノードがない時はローカル行列をワールド行列として渡す
						ReCalcNode->SetWorldMatrix(ReCalcNode->GetLocalMatrix());

						continue;
					}

					glm::mat4 NewWorldMatrix = ParentNode->GetWorldMatrix() * ReCalcNode->GetLocalMatrix();
					ReCalcNode->SetWorldMatrix(NewWorldMatrix);
				}

				// EndNodeの座標を更新
				EndPos = EndNode->GetWorldPos();

				// 接触しているなら終了
				if (glm::distance2(TargetPos, EndPos) < 0.01f)
				{
					// 終了
					DoLoop = false;
					break;
				}
			}

			// ループ回数を更新
			CurrentLoopNum++;
		}

		/*// 回転角度制限
		// 制限を行うことで例えば膝が変な方向に曲がらないようにする
		const int NumOfIKLink = static_cast<int>(m_IKParam->IKLinkList.size());

		for (int LinkIndex = NumOfIKLink - 1; LinkIndex >= 0; LinkIndex--)
		{
			const auto& IKLink = m_IKParam->IKLinkList[LinkIndex];

			// m_IKChainListにはIKLinkListの先頭から順番に入れている
			std::shared_ptr<object::CNode> ReCalcNode = m_IKChainList[NumOfIKLink - LinkIndex - 1];

			glm::quat rot = ReCalcNode->GetRot();
			
			if (IKLink.IsLimitAngle)
			{
				const auto& LowerAngle = IKLink.LowerAngle;
				const auto& UpperAngle = IKLink.UpperAngle;

				glm::vec3 euler = glm::eulerAngles(rot);

				// オイラー角に対して角度制限を行う
				// LowerAngleとUpperAngleはラジアン
				euler.x = glm::clamp(euler.x, LowerAngle.x, UpperAngle.x);
				euler.y = glm::clamp(euler.y, LowerAngle.y, UpperAngle.y);
				euler.z = glm::clamp(euler.z, LowerAngle.z, UpperAngle.z);

				rot = glm::quat(euler);
			}

			ReCalcNode->SetRot(rot);

			const auto& ParentNode = ReCalcNode->GetParentNode();
			if (!ParentNode)
			{
				// 親ノードがない時はローカル行列をワールド行列として渡す
				ReCalcNode->SetWorldMatrix(ReCalcNode->GetLocalMatrix());

				continue;
			}

			glm::mat4 NewWorldMatrix = ParentNode->GetWorldMatrix() * ReCalcNode->GetLocalMatrix();
			ReCalcNode->SetWorldMatrix(NewWorldMatrix);
		}*/

		return true;
	}
}
#endif