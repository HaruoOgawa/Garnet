#include "CDrawInfo.h"

namespace graphics
{
	CDrawInfo::CDrawInfo() :
		m_LightDir(glm::vec4(0.0f, 0.5f, 0.5f, 0.0f)),
		m_LightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	{
	}

	void CDrawInfo::SetLightDir(const glm::vec4& LightDir)
	{
		m_LightDir = LightDir;
	}

	const glm::vec4& CDrawInfo::GetLightDir() const
	{
		return m_LightDir;
	}

	void CDrawInfo::SetLightColor(const glm::vec4& LightColor)
	{
		m_LightColor = LightColor;
	}

	const glm::vec4& CDrawInfo::GetLightColor() const
	{
		return m_LightColor;
	}
}