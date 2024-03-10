#ifdef USE_ANIMATION

#include "CBlendShapeController.h"
#include "../Object/CNode.h"

namespace animation
{
	CBlendShapeController::CBlendShapeController():
		m_CurrentClipIndex(-1)
	{
	}

	CBlendShapeController::~CBlendShapeController()
	{
	}

	void CBlendShapeController::Reset()
	{
		// ウェイトをリセット
		ResetNodeWeights();

		m_CurrentClipIndex = -1;
	}

	void CBlendShapeController::AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip, const std::string& MotionName)
	{
		m_BlendShapeClip.push_back(Clip);
	}

	void CBlendShapeController::AddBlendShapeNode(const std::shared_ptr<object::CNode>& Node)
	{
		m_MorphNodeList.push_back(Node);
	}

	bool CBlendShapeController::Update(float DeltaSecondsTime)
	{
		// アニメーションの計算
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_BlendShapeClip.size())
		{
			const auto& Clip = m_BlendShapeClip[m_CurrentClipIndex];
			if (!Clip->Update(DeltaSecondsTime)) return false;

			// ウェイトを反映
			ApplyNodeWeights(Clip->GetCurrentMorphWeightMap());
		}

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

	void CBlendShapeController::ApplyNodeWeights(const std::map<EBlendShapeName, float>& CurrentMorphWeightMap)
	{
		// 複数BlendShapeClipに対応するときはCurrentMorphWeightMapをメンバ変数にすると良さそう？

		// mapからデータを取得する(map)
		std::vector<float> MorphWeights;

		// BlendShapeNameの順番で全て設定するようにする
		for (int i = 0; i < static_cast<int>(EBlendShapeName::Max); i++)
		{
			EBlendShapeName BlendShapeName = static_cast<EBlendShapeName>(i);

			auto it = CurrentMorphWeightMap.find(BlendShapeName);

			if (it != CurrentMorphWeightMap.end())
			{
				MorphWeights.push_back(it->second);
			}
			else
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

	// インデックス指定でモーションを変更
	void CBlendShapeController::ChangeBlendShape(int Index)
	{
		Reset();

		m_CurrentClipIndex = Index;
	}
}
#endif