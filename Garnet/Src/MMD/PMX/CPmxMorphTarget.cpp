#ifdef USE_MMD
#include "CPmxMorphTarget.h"

namespace mmd
{
	CPmxMorphTarget::CPmxMorphTarget(const std::pair<std::string, std::wstring>& MorphName, const std::pair<std::string, std::wstring>& MorphName_EN):
		m_MorphName(MorphName),
		m_MorphName_EN(MorphName_EN)
	{
	}

	// í∏ì_ÉÇÅ[Ét
	void CPmxMorphTarget::AddVertexMorph(const SPmxVertexMorph& VertexMorph)
	{
		m_VertexMorphList.push_back(VertexMorph);
	}

	const std::vector<SPmxVertexMorph>& CPmxMorphTarget::GetVertexMorphList() const
	{
		return m_VertexMorphList;
	}
}
#endif