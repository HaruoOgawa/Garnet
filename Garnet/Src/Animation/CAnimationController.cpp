#ifdef USE_ANIMATION

#include "CAnimationController.h"
#include "../../Debug/Message/Console.h"

namespace animation
{
	CAnimationController::CAnimationController():
		m_MaxBlendingTime(0.5f),
		m_CurrBlendingTime(0.0f),
		m_SavedPrevTrs(false),
		m_Skeleton(nullptr),
		m_CurrentClipIndex(-1),
		m_CurrentClipName("")
	{
	}

	CAnimationController::~CAnimationController()
	{
		Reset();
	}

	void CAnimationController::Reset()
	{
		m_CurrBlendingTime = 0.0f;
		m_SavedPrevTrs = false;
		m_CurrentClipIndex = -1;
		m_CurrentClipName = "";
	}

	bool CAnimationController::Update(float DeltaSecondsTime)
	{
		// アニメーションの計算
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			const auto& Clip = m_ClipList[m_CurrentClipIndex];
			if (!Clip->Update(DeltaSecondsTime)) return false;

			// モーションブレンド
			if (!BlendMotion(DeltaSecondsTime)) return false;
		}
		else if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			const auto& Layout = m_ClipMap.find(m_CurrentClipName);
			if (Layout != m_ClipMap.end())
			{
				const auto& Clip = Layout->second.Clip;

				if (Clip->IsEnd() && !Clip->IsLoop())
				{
					// アニメーションが終了しているので次のアニメーションに遷移する
					const std::string& NextClipName = Layout->second.NextClipName;

					if (!NextClipName.empty())
					{
						ChangeMotion(NextClipName);

						return true;
					}
				}
				else
				{
					if (!Clip->Update(DeltaSecondsTime)) return false;

					// モーションブレンド
					if (!BlendMotion(DeltaSecondsTime)) return false;
				}
			}
		}

		return true;
	}

	// IKの計算
	bool CAnimationController::CalculateIK(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		if (m_Skeleton)
		{
			if (!m_Skeleton->SolveIK()) return false;
		}

		//if (!DoCCDIK(NodeList)) return false;

		return true;
	}

	bool CAnimationController::DoCCDIK(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		if (m_Skeleton)
		{
			const auto& BoneList = m_Skeleton->GetBoneList();

			for (const auto& IKBone : m_Skeleton->GetIKBoneList())
			{
				// TargetBoneが目指すボーンの位置
				glm::vec3 IKGoalPos = glm::vec3(0.0f);
				math::CTransform::CastModelMatrixToTranslation(IKBone->GetBoneNode()->GetWorldMatrix(), IKGoalPos);

				glm::vec4 TestIKPos = IKBone->GetBoneNode()->GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

				const auto& IKParam = IKBone->GetIKParam();

				// Linkが一つもなければスキップ
				if (IKParam->IKLinkList.size() == 0) continue;

				// 足首がおかしくなるのでひとまずLinkBoneが1つの時も計算しない(必要になったら対応する)
				if (IKParam->IKLinkList.size() == 1) continue;

				// IKターゲットボーン
				if (IKParam->IKTargetBoneIndex < 0 || IKParam->IKTargetBoneIndex >= BoneList.size()) continue;
				const auto& EndEffectorBone = BoneList[IKParam->IKTargetBoneIndex];
				
				glm::vec3 EndEffectorPos = glm::vec3(0.0f);
				math::CTransform::CastModelMatrixToTranslation(EndEffectorBone->GetBoneNode()->GetWorldMatrix(), EndEffectorPos);

				// CCD-IKを採用
				// CCD-IKに使用するサイクリックボーンリスト
				std::vector<std::shared_ptr<CBone>> LinkBoneList;
				std::vector<glm::vec3> LinkPosList;

				for (const auto& Link : IKParam->IKLinkList)
				{
					if (Link.IKLinkBoneIndex == -1) continue;

					const auto& CyclicBone = BoneList[Link.IKLinkBoneIndex];

					LinkBoneList.push_back(CyclicBone);

					glm::vec3 CyclicPos = glm::vec3(0.0f);
					math::CTransform::CastModelMatrixToTranslation(CyclicBone->GetBoneNode()->GetWorldMatrix(), CyclicPos);

					LinkPosList.push_back(CyclicPos);
				}

				// サイクルスタート
				if (LinkBoneList.size() > 0)
				{
					// どれくらい近づいたらターゲットに届いたと判定するかの閾値
					const float CyclicThreshold = 0.01f;

					{
						glm::mat4 TestMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 0.0f));

						glm::vec4 TestPosA = TestMatrix * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

						//TestMatrix = TestMatrix * glm::mat4_cast(glm::angleAxis(3.1415f, glm::vec3(0.0f, 0.0f, 1.0f)));
						TestMatrix = TestMatrix * glm::mat4_cast(glm::angleAxis(3.1415f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)));

						glm::vec4 TestPosB = TestMatrix * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

						float x = 0.0f;
					}

					// ターゲットに届くかサイクルの最大値に達するまで計算を繰り返す
					int CurrentLoopNum = 0;
					while (CurrentLoopNum < IKParam->IKLoopCount)
					{
						bool Result = false;

						for (int LinkIndex = 0; LinkIndex < LinkBoneList.size(); LinkIndex++)
						{
							const auto& LinkBone = LinkBoneList[LinkIndex];
							const auto& LinkNode = LinkBone->GetBoneNode();

							glm::vec3 LinkPos = LinkPosList[LinkIndex];

							// 回転行列を計算
							glm::vec3 ToEndEffectorVector = glm::normalize(EndEffectorPos - LinkPos);
							glm::vec3 ToTargetVector = glm::normalize(IKGoalPos - LinkPos);

							// ここから先が違う /////////////////////////////////////////////
							// 回転軸となるJoint(CurrJointPos)を中心にJoint以降のノードを回転させる
							// https://cnc-selfbuild.blogspot.com/2021/03/inverse-kinematics-backward.html
							
							glm::quat CCDRot = math::CTransform::CalcTwoVectorRotate(ToEndEffectorVector, ToTargetVector, IKParam->LimitedAngle);
							//glm::quat CCDRot = math::CTransform::CalcTwoVectorRotate(ToTargetVector, ToEndEffectorVector, IKParam->LimitedAngle);
							//glm::mat4 LinkInverseWorldMatrix = glm::inverse(LinkNode->GetWorldMatrix());

							{
								glm::vec3 LocalPos = EndEffectorPos - LinkPos;
								glm::vec4 RotPos = glm::mat4_cast(CCDRot) * glm::vec4(LocalPos.x, LocalPos.y, LocalPos.z, 1.0f);
								LocalPos = glm::vec3(RotPos.x, RotPos.y, RotPos.z);

								EndEffectorPos = LinkPos + LocalPos;

								EndEffectorBone->GetBoneNode()->SetWorldMatrix(glm::translate(glm::mat4(1.0f), EndEffectorPos));

								/*const auto& EndEffectorNode = EndEffectorBone->GetBoneNode();

								// 現在の回転軸を中心としたローカル座標を取得
								math::CTransform LocalTransform = math::CTransform(LinkInverseWorldMatrix * EndEffectorNode->GetWorldMatrix());
								//LocalTransformA.MulRot(CCDRot);

								glm::vec4 LocalPos =
									glm::mat4_cast(CCDRot * LocalTransform.GetRot()) *
									glm::vec4(LocalTransform.GetPos().x, LocalTransform.GetPos().y, LocalTransform.GetPos().z, 1.0f);

								// 現在の回転軸を中心にワールド座標を再計算
								//glm::mat4 WorldMatrix = LinkNode->GetWorldMatrix() * LocalTransform.GetModelMatrix();
								glm::mat4 WorldMatrix = LinkNode->GetWorldMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(LocalPos.x, LocalPos.y, LocalPos.z));

								EndEffectorNode->SetWorldMatrix(WorldMatrix);*/
							}

							for (int CalcIndex = 0; CalcIndex < LinkIndex; CalcIndex++)
							{
								glm::vec3 LocalPos = LinkPosList[CalcIndex] - LinkPos;
								glm::vec4 RotPos = glm::mat4_cast(CCDRot) * glm::vec4(LocalPos.x, LocalPos.y, LocalPos.z, 1.0f);
								LocalPos = glm::vec3(RotPos.x, RotPos.y, RotPos.z);

								LinkPosList[CalcIndex] = LinkPos + LocalPos;

								LinkBoneList[CalcIndex]->GetBoneNode()->SetWorldMatrix(glm::translate(glm::mat4(1.0f), LinkPosList[CalcIndex]));

								/*auto& CalcBone = LinkBoneList[CalcIndex];
								const auto& CalcNode = CalcBone->GetBoneNode();

								// 現在の回転軸を中心としたローカル座標を取得
								math::CTransform LocalTransform = math::CTransform(LinkInverseWorldMatrix * CalcNode->GetWorldMatrix());
								//LocalTransformA.MulRot(CCDRot);

								glm::vec4 LocalPos =
									glm::mat4_cast(CCDRot * LocalTransform.GetRot()) *
									glm::vec4(LocalTransform.GetPos().x, LocalTransform.GetPos().y, LocalTransform.GetPos().z, 1.0f);

								// 現在の回転軸を中心にワールド座標を再計算
								//glm::mat4 WorldMatrix = LinkNode->GetWorldMatrix() * LocalTransform.GetModelMatrix();
								glm::mat4 WorldMatrix = LinkNode->GetWorldMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(LocalPos.x, LocalPos.y, LocalPos.z));

								CalcNode->SetWorldMatrix(WorldMatrix);*/
							}

							/*glm::quat Rot = math::CTransform::CalcTwoVectorRotate(ToEndEffectorVector, ToTargetVector, IKParam->LimitedAngle);

							{
								// SecondLinkPosの位置のボーンの回転を更新する(自動的に子要素も回転するので便利)
								glm::quat LinkRot = LinkBone->GetBoneNode()->GetRot();
								LinkRot *= Rot;

								// 角度制限を行うかどうか
								if (IKParam->IKLinkList[LinkIndex].IsLimitAngle)
								{
									math::CTransform::ClampRotate(LinkRot, IKParam->IKLinkList[LinkIndex].LowerAngle, IKParam->IKLinkList[LinkIndex].UpperAngle);
								}

								LinkBone->GetBoneNode()->SetRot(LinkRot);

								// CyclicBoneのワールド行列を更新
								glm::mat4 WorldMatrix = LinkBone->GetBoneNode()->GetParentNode()->GetWorldMatrix() * LinkBone->GetBoneNode()->GetLocalMatrix();
								LinkBone->GetBoneNode()->SetWorldMatrix(WorldMatrix);

								// 子要素の行列を再計算
								for (int ChildIndex : LinkBone->GetBoneNode()->GetChildrenNodeIndexList())
								{
									if (ChildIndex < 0 || ChildIndex >= NodeList.size()) continue;

									const auto& ChildNode = NodeList[ChildIndex];

									CalcWorldMatrix(WorldMatrix, ChildNode, NodeList);
								}
							}*/

							// 計算結果を見て末端のボーンがIKBoneにどれくらい近づいたか見る
							//glm::vec3 CyclicResultPos = glm::vec3(0.0f);
							//math::CTransform::CastModelMatrixToTranslation(EndEffectorBone->GetBoneNode()->GetWorldMatrix(), CyclicResultPos);

							//if (glm::distance(IKGoalPos, CyclicResultPos) < CyclicThreshold)
							if (glm::distance(IKGoalPos, EndEffectorPos) < CyclicThreshold)
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
				}
			}
		}

		return true;
	}

	// 付与ボーンの計算
	bool CAnimationController::CalculateGrantBone(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		// 付与はローカルトランスフォームに対して実行する
		if(m_Skeleton)
		{
			const auto& BoneList = m_Skeleton->GetBoneList();

			for (const auto& GrantBone : m_Skeleton->GetGrantBoneList())
			{
				// ParentGrantBoneを取得
				int GrantParentBoneIndex = GrantBone->GetGrantParentBoneIndex();
				if (GrantParentBoneIndex < 0 || GrantParentBoneIndex >= BoneList.size()) continue;

				const auto& ParentGrantBone = BoneList[GrantParentBoneIndex];
				if (!ParentGrantBone) continue;

				// 付与率
				const float GrantRate = GrantBone->GetGrantRate();

				// ひとまず負の時はスキップする
				if (GrantRate < 0.0f) continue;

				if (GrantBone->IsRotateGrant())
				{
					// 回転付与

					glm::quat LocalParentRot = ParentGrantBone->GetBoneNode()->GetRot();

					if (GrantRate >= 0.0f)
					{
						//glm::quat GrantRot = (GrantRate * LocalParentRot) * GrantBone->GetBoneNode()->GetRot();
						glm::quat GrantRot = glm::slerp(GrantBone->GetBoneNode()->GetRot(), LocalParentRot, GrantRate);

						GrantBone->GetBoneNode()->SetRot(GrantRot);
					}
					else
					{
						// 付与率が負の時は逆行列をかける
						glm::quat GrantRot = (fabsf(GrantRate) * glm::inverse(LocalParentRot)) * GrantBone->GetBoneNode()->GetRot();

						GrantBone->GetBoneNode()->SetRot(GrantRot);
					}
				}
				else if (GrantBone->IsMoveGrant())
				{
					glm::vec3 LocalParentPos = ParentGrantBone->GetBoneNode()->GetPos();

					// 移動付与
					if (GrantRate >= 0.0f)
					{
						//glm::vec3 GrantPos = GrantRate * LocalParentPos + GrantBone->GetBoneNode()->GetPos();
						glm::vec3 GrantPos = (1.0f - GrantRate) * GrantBone->GetBoneNode()->GetPos() + GrantRate * LocalParentPos;

						GrantBone->GetBoneNode()->SetPos(GrantPos);
					}
					else
					{
						// 付与率が負の時は逆行列をかける
						glm::vec3 GrantPos = (-1.0f) * GrantRate * LocalParentPos + GrantBone->GetBoneNode()->GetPos();

						GrantBone->GetBoneNode()->SetPos(GrantPos);
					}
				}
			}
		}

		return true;
	}

	void CAnimationController::CalcWorldMatrix(const glm::mat4& ParentWorldMatrix, const std::shared_ptr<object::CNode>& Node, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		glm::mat4 WorldMatrix = ParentWorldMatrix * Node->GetLocalMatrix();
		Node->SetWorldMatrix(WorldMatrix);

		for (int ChildIndex : Node->GetChildrenNodeIndexList())
		{
			if (ChildIndex < 0 || ChildIndex >= NodeList.size()) continue;

			const auto& ChildNode = NodeList[ChildIndex];

			CalcWorldMatrix(WorldMatrix, ChildNode, NodeList);
		}
	}

	// インデックス指定でモーションを変更
	void CAnimationController::ChangeMotion(int Index)
	{
		Reset();

		m_CurrentClipIndex = Index;

		// 初期化
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			const auto& Clip = m_ClipList[m_CurrentClipIndex];
			Clip->Initialize();
		}
	}

	// 名前指定でモーションを変更
	void CAnimationController::ChangeMotion(const std::string& MotionName)
	{
		Reset();

		m_CurrentClipName = MotionName;

		// 初期化
		if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			const auto& Clip = m_ClipMap.find(m_CurrentClipName);
			if (Clip != m_ClipMap.end())
			{
				Clip->second.Clip->Initialize();
			}
		}

		// 現在の姿勢を保存する
		if (!m_SavedPrevTrs)
		{
			if(m_Skeleton)
			{
				for (const auto& Bone : m_Skeleton->GetBoneList())
				{
					Bone->GetBoneNode()->SavePrevLocalTransform();
				}
			}

			m_SavedPrevTrs = true;
		}
	}

	bool CAnimationController::CalCSkinMatrixList(std::vector<glm::mat4>& MatrixList, const glm::mat4& ObjectModelMatrix)
	{
		//if (IsPlayingAnimation())
		if (IsEnabledSkeleton())
		{
			if(m_Skeleton)
			{
				if (!m_Skeleton->CalCSkinMatrixList(MatrixList, ObjectModelMatrix)) return false;
			}
		}

		return true;
	}

	bool CAnimationController::IsEnabledSkeleton()
	{
		return (m_Skeleton != nullptr && m_Skeleton->GetBoneList().size() > 0);
	}

	bool CAnimationController::IsPlayingAnimation()
	{
		bool result = false;

		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_ClipList.size())
		{
			result = true;
		}
		else if (m_ClipMap.find(m_CurrentClipName) != m_ClipMap.end())
		{
			result = true;
		}

		return result;
	}

	void CAnimationController::AddMotion(const std::string& MotionName, animation::SAnimationLayout Layout)
	{
		m_ClipMap.emplace(MotionName, Layout);
		m_ClipList.push_back(Layout.Clip);
	}

	void CAnimationController::SetAnimationSkeleton(const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		m_Skeleton = Skeleton;
	}

	const std::shared_ptr<animation::CSkeleton>& CAnimationController::GetSkeleton() const
	{
		return m_Skeleton;
	}

	void CAnimationController::AddAnimationClip(const std::shared_ptr<animation::CAnimationClip>& Clip)
	{
		m_ClipList.push_back(Clip);
	}

	void CAnimationController::AddHumanoidAnimationClip(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::string& MotionName, animation::SAnimationLayout Layout, bool IsLoop)
	{
		if (!m_Skeleton) return;

		// Clipの値をコピーする
		std::shared_ptr<animation::CAnimationClip> TargetClip = std::make_shared<animation::CAnimationClip>();

		// samplers
		for (const auto& SourceSampler : SourceClip->GetSamplerList())
		{
			std::shared_ptr<animation::CAnimationSampler> TargetSampler = std::make_shared<animation::CAnimationSampler>(SourceSampler->GetInterpolationType());

			for (const auto& SourceKeyFrame : SourceSampler->GetKeyFrameList())
			{
				std::shared_ptr<animation::CKeyFrame> TargetKeyFrame = std::make_shared<animation::CKeyFrame>(SourceKeyFrame->GetType());

				TargetKeyFrame->SetInput(SourceKeyFrame->GetInput());

				std::vector<float> TargetOutput = SourceKeyFrame->GetOutput();
				TargetKeyFrame->SetOutput(TargetOutput);

				TargetKeyFrame->SetXPointList(SourceKeyFrame->GetXPointList());
				TargetKeyFrame->SetYPointList(SourceKeyFrame->GetYPointList());
				TargetKeyFrame->SetZPointList(SourceKeyFrame->GetZPointList());
				TargetKeyFrame->SetRPointList(SourceKeyFrame->GetRPointList());

				TargetSampler->AddKeyFrame(TargetKeyFrame);
			}

			TargetSampler->SetStartTime(SourceSampler->GetStartTime());
			TargetSampler->SetEndTime(SourceSampler->GetEndTime());

			TargetClip->AddAnimationSampler(TargetSampler);
		}

		// channels
		// 同じ名前のノードは一つしかない前提でchannelを作成する
		for (const auto& SourceChannel : SourceClip->GetChannelList())
		{
			std::shared_ptr<object::CNode> TargetNode = nullptr;

			for (const auto& Bone : m_Skeleton->GetBoneList())
			{
				if (Bone->GetBoneName() == animation::EHumanoidBones::None) continue;

				if (Bone->GetBoneName() == SourceChannel->GetBoneName())
				{
					TargetNode = Bone->GetBoneNode();

					break;
				}
			}

			std::shared_ptr<animation::CAnimationChannel> TargetChannel = std::make_shared<animation::CAnimationChannel>(SourceChannel->IsUseAnimLocalAxis(), SourceChannel->IsTransOffset(), SourceChannel->GetSamplerIndex(), SourceChannel->GetAnimationTarget(), TargetNode, SourceChannel->GetBoneName());

			TargetClip->AddAnimationChannel(TargetChannel);
		}

		// DefaultSkeletonを持っている時のみリターゲットを行う
		// リターゲットは平行移動成分(Pos)に対して行うものなので、回転だけのアニメーションには必要ない
		if (SourceClip->GetDefaultSkeleton())
		{
			// RigのReTargetingを行う
			// リターゲティングとはリグの形が異なるアニメーションを自身のアニメーションに合うように調整すること
			// 例えば身長が違うとアバターが伸びてしまう
			if (!ReTargetRig(SourceClip, TargetClip)) return;
		}

		TargetClip->SetIsLoop(IsLoop);

		Layout.Clip = TargetClip;
		AddMotion(MotionName, Layout);
	}

	const std::vector<std::shared_ptr<animation::CAnimationClip>>& CAnimationController::GetAnimationClipList() const
	{
		return m_ClipList;
	}

	const std::unordered_map<std::string, SAnimationLayout>& CAnimationController::GetAnimationClipMap() const
	{
		return m_ClipMap;
	}

	bool CAnimationController::BlendMotion(float DeltaSecondsTime)
	{
		if (m_CurrBlendingTime < m_MaxBlendingTime)
		{
			// 時間更新
			m_CurrBlendingTime += DeltaSecondsTime;

			m_CurrBlendingTime = fmaxf(m_CurrBlendingTime, 0.0f);
			m_CurrBlendingTime = fminf(m_CurrBlendingTime, m_MaxBlendingTime);

			// 現在の姿勢と遷移前の姿勢を補完する
			std::vector<std::shared_ptr<object::CNode>> ComputedNodeList;
			float L = 1.0f - (m_MaxBlendingTime - m_CurrBlendingTime) / m_MaxBlendingTime;

			if(m_Skeleton)
			{
				for (const auto& Bone : m_Skeleton->GetBoneList())
				{
					const auto& Node = Bone->GetBoneNode();

					BlendTranslation(Node, L);
					BlendRotation(Node, L);
				}
			}
		}

		return true;
	}

	void CAnimationController::BlendTranslation(const std::shared_ptr<object::CNode>& Node, float L)
	{
		const glm::vec3& PrevPos = Node->GetPrevLocalTransform()->GetPos();
		const glm::vec3& NextPos = Node->GetLocalTransform()->GetPos();
		
		glm::vec3 CurrPos = glm::vec3(0.0f);

		CurrPos.x = (1.0f - L) * PrevPos.x + L * NextPos.x;
		CurrPos.y = (1.0f - L) * PrevPos.y + L * NextPos.y;
		CurrPos.z = (1.0f - L) * PrevPos.z + L * NextPos.z;

		Node->GetLocalTransform()->SetPos(CurrPos);
	}

	void CAnimationController::BlendRotation(const std::shared_ptr<object::CNode>& Node, float L)
	{
		const glm::quat& PrevRot = Node->GetPrevLocalTransform()->GetRot();
		const glm::quat& NextRot = Node->GetLocalTransform()->GetRot();

		glm::quat CurrRot = glm::slerp(PrevRot, NextRot, L);

		Node->GetLocalTransform()->SetRot(CurrRot);
	}

	bool CAnimationController::ReTargetRig(const std::shared_ptr<animation::CAnimationClip>& SourceClip, const std::shared_ptr<animation::CAnimationClip>& TargetClip)
	{
		const auto& SourceSkeleton = SourceClip->GetDefaultSkeleton();
		if (!SourceSkeleton) return false;

		// Rigのリターゲティングを実行する
		for (const auto& TargetChannel : TargetClip->GetChannelList())
		{
			int TargetSamplerIndex = TargetChannel->GetSamplerIndex();
			if (TargetSamplerIndex < 0 || TargetSamplerIndex >= TargetClip->GetSamplerList().size()) continue;

			const auto& TargetSampler = TargetClip->GetSamplerList()[TargetSamplerIndex];

			animation::EHumanoidBones BoneName = TargetChannel->GetBoneName();

			animation::EAnimationTarget AnimationTarget = TargetChannel->GetAnimationTarget();

			// BoneTableに登録されていないものについては処理の対象外とする
			if (BoneName == animation::EHumanoidBones::None) continue;

			const auto& SourceBone = SourceSkeleton->GetBone(BoneName);
			if (!SourceBone) continue;

			const glm::mat4 SourceRestMove = SourceBone->GetBoneNode()->GetDefaultLocalMoveMatrix();
			const glm::mat4 InverseSourceRestMove = glm::inverse(SourceRestMove);

			const auto& TargetBone = m_Skeleton->GetBone(BoneName);
			if (!TargetBone) continue;

			const glm::mat4 TargetRestMove = TargetBone->GetBoneNode()->GetDefaultLocalMoveMatrix();

			// SourceとTargetのバインドマトリックスのTranslationの差分を示す行列
			const glm::mat4 ReTargetTranslationMatrix = TargetRestMove * InverseSourceRestMove;

			for (const auto& TargetKeyFrame : TargetSampler->GetKeyFrameList())
			{
				const float CurrentTime = TargetKeyFrame->GetInput();

				if (AnimationTarget == animation::EAnimationTarget::MODELMATRIX)
				{
					glm::mat4 SourcePose = glm::mat4(1.0f);
					TargetKeyFrame->GetOutput(&SourcePose[0][0]);

					glm::mat4 TargetPose = ReTargetTranslationMatrix * SourcePose;

					TargetKeyFrame->SetOutput(&TargetPose[0][0], sizeof(glm::mat4));
				}
				else if (AnimationTarget == animation::EAnimationTarget::TRANSLATION)
				{
					glm::vec3 SourceTranslation = glm::vec3(1.0f);
					TargetKeyFrame->GetOutput(&SourceTranslation[0]);

					glm::mat4 SourcePose = glm::translate(glm::mat4(1.0f), SourceTranslation);

					glm::mat4 TargetPose = ReTargetTranslationMatrix * SourcePose;

					glm::vec3 TargetTranslation = glm::vec3(1.0f);
					math::CTransform::CastModelMatrixToTranslation(TargetPose, TargetTranslation);

					TargetKeyFrame->SetOutput(&TargetTranslation[0], sizeof(glm::vec3));
				}
				else
				{
					// リターゲットはリグの長さの違いを補正するためのものなのでMODELMATRIXとTRANSLATIONに対してのみ行う
					continue;
				}
			}
		}

		return true;
	}
}

#endif