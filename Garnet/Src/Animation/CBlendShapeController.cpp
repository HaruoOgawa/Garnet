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

	void CBlendShapeController::Reset(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		// ウェイトをリセット
		ResetNodeWeights(NodeList);

		m_CurrentClipIndex = -1;
	}

	void CBlendShapeController::AddBlendShapeClip(const std::shared_ptr<animation::CBlendShapeClip>& Clip)
	{
		m_BlendShapeClip.push_back(Clip);
	}

	bool CBlendShapeController::Update(float DeltaSecondsTime, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		// アニメーションの計算
		if (m_CurrentClipIndex >= 0 && m_CurrentClipIndex < m_BlendShapeClip.size())
		{
			const auto& Clip = m_BlendShapeClip[m_CurrentClipIndex];
			if (!Clip->Update(DeltaSecondsTime)) return false;

			// ウェイトを反映
			//ApplyNodeWeights(Clip->GetCurrentMorphWeights(), NodeList);
		}

		return true;
	}

	void CBlendShapeController::ResetNodeWeights(const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& Node : NodeList)
		{
			if (!Node || Node->GetMeshIndex() == -1) continue;

			Node->ClearMorphWeights();
		}
	}

	void CBlendShapeController::ApplyNodeWeights(const std::vector<float>& MorphWeights, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& Node : NodeList)
		{
			if (!Node || Node->GetMeshIndex() == -1) continue;

			Node->ClearMorphWeights();
			Node->SetMorphWeight(MorphWeights);
		}
	}

	// インデックス指定でモーションを変更
	void CBlendShapeController::ChangeBlendShape(int Index, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		Reset(NodeList);

		m_CurrentClipIndex = Index;
	}
}
#endif