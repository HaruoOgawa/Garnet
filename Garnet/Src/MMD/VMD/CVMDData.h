#pragma once
#if defined(USE_MMD)  && defined(USE_BINARY_READ)
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "SVMDFrame.h"
#include "../../Animation/EHumanoidBones.h"
#include "../../Animation/EBlendShapeName.h"

namespace binary { class CBinaryReader; }

namespace mmd
{
	class CVMDData
	{
		// ボーンアニメーション
		std::map<animation::EHumanoidBones, std::vector<SVMDFrame>> m_FrameMap;
		int m_MinFrameIndex;
		int m_MaxFrameIndex;

		// 表情アニメーション
		std::map<animation::EBlendShapeName, std::vector<SVMDSkinFrame>> m_SkinFrameMap;
		int m_MinSkinFrameIndex;
		int m_MaxSkinFrameIndex;
	private:
		// 他にもExpression, Camera, LightなどがあるらしいがひとまずFrameだけ読む
		bool AnalyseFrameData(binary::CBinaryReader& Analyser);
		bool AnalyseFacialExpressionData(binary::CBinaryReader& Analyser);
		bool AnalyseCameraData(binary::CBinaryReader& Analyser);
		bool AnalyseLightData(binary::CBinaryReader& Analyser);
		bool AnalyseSelfShadowData(binary::CBinaryReader& Analyser);
	public:
		CVMDData();
		virtual ~CVMDData();

		// ボーンアニメーション
		const std::map<animation::EHumanoidBones, std::vector<SVMDFrame>>& GetFrameMap() const;
		int GetMinFrameIndex() const;
		int GetMaxFrameIndex() const;

		// 表情アニメーション
		const std::map<animation::EBlendShapeName, std::vector<SVMDSkinFrame>>& GetSkinFrameMap() const;
		int GetMinSkinFrameIndex() const;
		int GetMaxSkinFrameIndex() const;

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif