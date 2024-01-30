#pragma once
#ifdef USE_MMD
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "SVMDFrame.h"

namespace binary { class CBinaryAnalyser; }

namespace mmd
{
	class CVMDData
	{
		std::map<std::wstring, std::vector<SVMDFrame>> m_FrameMap;
		int m_MinFrameIndex;
		int m_MaxFrameIndex;
	private:
		// ‘¼‚É‚àExpression, Camera, Light‚È‚Ç‚ª‚ ‚é‚ç‚µ‚¢‚ª‚Ð‚Æ‚Ü‚¸Frame‚¾‚¯“Ç‚Þ
		bool AnalyseFrameData(binary::CBinaryAnalyser& Analyser);
	public:
		CVMDData();
		virtual ~CVMDData();

		const std::map<std::wstring, std::vector<SVMDFrame>>& GetFrameMap() const;
		int GetMinFrameIndex() const;
		int GetMaxFrameIndex() const;

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif