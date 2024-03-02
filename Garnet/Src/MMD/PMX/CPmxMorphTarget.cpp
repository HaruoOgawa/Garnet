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
	void CPmxMorphTarget::AddVertexMorph(int VertexIndex, const glm::vec3& Offset)
	{
		m_VertexMorphList.emplace(VertexIndex, Offset);
	}

	const std::map<int, glm::vec3>& CPmxMorphTarget::GetVertexMorphList() const
	{
		return m_VertexMorphList;
	}
}
#endif