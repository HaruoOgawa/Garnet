#ifdef USE_ANIMATION

#include "CBlendShapeController.h"
#include "../Object/CNode.h"

namespace animation
{
	CBlendShapeController::CBlendShapeController()
	{
	}

	CBlendShapeController::~CBlendShapeController()
	{
	}

	void CBlendShapeController::Reset()
	{
		// ウェイトをリセット
		ResetNodeWeights();
	}

	void CBlendShapeController::AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName, bool IsLoop)
	{
		Clip->SetIsLoop(IsLoop);

		m_BlendShapeClipMap.emplace(MotionName, Clip);
	}

	void CBlendShapeController::AddBlendShapeNode(const std::shared_ptr<object::CNode>& Node)
	{
		m_MorphNodeList.push_back(Node);
	}

	bool CBlendShapeController::Update(float DeltaSecondsTime)
	{
		if (m_PlayingBlendShapeSet.size() == 0) return true;

		// 初期化
		m_MorphWeightsList.clear();

		// アニメーションの計算
		for (const auto& ClipName : m_PlayingBlendShapeSet)
		{
			auto it = m_BlendShapeClipMap.find(ClipName);

			if (it != m_BlendShapeClipMap.end())
			{
				auto& Clip = it->second;
				if (!Clip->Update(DeltaSecondsTime)) return false;

				m_MorphWeightsList.push_back(Clip->GetCurrentMorphWeightMap());
			}
		}

		// ウェイトを反映
		ApplyNodeWeights();

		return true;
	}

	void CBlendShapeController::ResetNodeWeights()
	{
		for (const auto& Node : m_MorphNodeList)
		{
			if (!Node || Node->GetMeshIndex() == -1) continue;

			Node->ClearMorphWeights();
		}
	}

	void CBlendShapeController::ApplyNodeWeights()
	{
		// mapからデータを取得する(map)
		std::vector<float> MorphWeights;

		// BlendShapeNameの順番で全て設定するようにする
		for (int i = 0; i < static_cast<int>(EBlendShapeName::Max); i++)
		{
			EBlendShapeName BlendShapeName = static_cast<EBlendShapeName>(i);

			bool FindResult = false;

			for (const auto& CurrentMorphWeightMap : m_MorphWeightsList)
			{
				auto it = CurrentMorphWeightMap.find(BlendShapeName);

				if (it != CurrentMorphWeightMap.end())
				{
					MorphWeights.push_back(it->second);

					FindResult = true;

					break;
				}
			}

			// 見つからなければ初期値を入れる
			if (!FindResult)
			{
				MorphWeights.push_back(0.0f);
			}
		}

		for (const auto& Node : m_MorphNodeList)
		{
			if (!Node || Node->GetMeshIndex() == -1) continue;

			Node->ClearMorphWeights();
			Node->SetMorphWeight(MorphWeights);
		}
	}

	void CBlendShapeController::PlayBlendShape(const std::string& MotionName)
	{
		m_PlayingBlendShapeSet.emplace(MotionName);
	}

	void CBlendShapeController::StopBlendShape(const std::string& MotionName)
	{
		m_PlayingBlendShapeSet.erase(MotionName);
	}
}
#endif