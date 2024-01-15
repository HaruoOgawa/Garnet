#ifdef USE_MMD
#include "CPmxMaterial.h"

namespace mmd
{
	CPmxMaterial::CPmxMaterial(std::pair<std::string, std::wstring> MaterialName, std::pair<std::string, std::wstring> MaterialName_EN, glm::vec4 Diffuse, glm::vec3 Specular, float SpecularCoef, glm::vec3 Ambient,
		unsigned char DrawBitFlag, glm::vec4 EdgeColor, float EdgeSize, int MainTexIndex, int SphereTexIndex, EPmxSphereMode SphereMode, int ToonTexIndex, int SharedToonTexIndex,
		std::pair<std::string, std::wstring> MaterialDescription, int MatRefIndiceOffset) :
		m_MaterialName(MaterialName),
		m_MaterialName_EN(MaterialName_EN),
		m_Diffuse(Diffuse),
		m_Specular(Specular),
		m_SpecularCoef(SpecularCoef),
		m_Ambient(Ambient),
		m_DrawDoubleSlided(false),
		m_DrawGroundShadow(false),
		m_DrawSelfShadowMap(false),
		m_DrawSelfShadow(false),
		m_DrawEdge(false),
		m_EdgeColor(EdgeColor),
		m_EdgeSize(EdgeSize),
		m_MainTexIndex(MainTexIndex),
		m_SphereTexIndex(SphereTexIndex),
		m_SphereMode(SphereMode),
		m_ToonTexIndex(ToonTexIndex),
		m_SharedToonTexIndex(SharedToonTexIndex),
		m_MaterialDescription(MaterialDescription),
		m_MatRefIndiceOffset(MatRefIndiceOffset)
	{
		AnalyseDrawBitFlag(DrawBitFlag);
	}

	void CPmxMaterial::AnalyseDrawBitFlag(unsigned char DrawBitFlag)
	{
		m_DrawDoubleSlided = (DrawBitFlag & 0x01);
		m_DrawGroundShadow = (DrawBitFlag & 0x02);
		m_DrawSelfShadowMap = (DrawBitFlag & 0x04);
		m_DrawSelfShadow = (DrawBitFlag & 0x08);
		m_DrawEdge = (DrawBitFlag & 0x10);
	}
}
#endif