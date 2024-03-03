#pragma once
#ifdef USE_MMD
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "SVMDFrame.h"
#include "../../Animation/EHumanoidBones.h"
#include "../../Animation/EBlendShapeName.h"

namespace binary { class CBinaryAnalyser; }

namespace mmd
{
	class CVMDData
	{
		std::map<animation::EHumanoidBones, std::vector<SVMDFrame>> m_FrameMap;
		int m_MinFrameIndex;
		int m_MaxFrameIndex;

		// ï\èÓ
		std::map<std::wstring, std::vector<SVMDSkinFrame>> m_SkinFrameMap;
	private:
		// ëºÇ…Ç‡Expression, Camera, LightÇ»Ç«Ç™Ç†ÇÈÇÁÇµÇ¢Ç™Ç–Ç∆Ç‹Ç∏FrameÇæÇØì«Çﬁ
		bool AnalyseFrameData(binary::CBinaryAnalyser& Analyser);
		bool AnalyseFacialExpressionData(binary::CBinaryAnalyser& Analyser);
		bool AnalyseCameraData(binary::CBinaryAnalyser& Analyser);
		bool AnalyseLightData(binary::CBinaryAnalyser& Analyser);
		bool AnalyseSelfShadowData(binary::CBinaryAnalyser& Analyser);
	public:
		CVMDData();
		virtual ~CVMDData();

		const std::map<animation::EHumanoidBones, std::vector<SVMDFrame>>& GetFrameMap() const;
		int GetMinFrameIndex() const;
		int GetMaxFrameIndex() const;

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif