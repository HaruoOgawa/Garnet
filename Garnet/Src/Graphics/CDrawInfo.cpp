#include "CDrawInfo.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"

namespace graphics
{
	CDrawInfo::CDrawInfo() :
		m_LightCamera(std::make_shared<camera::CCamera>()),
		m_LightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
		m_LightProjection(std::make_shared<projection::CProjection>())
	{
	}

	void CDrawInfo::SetLightCamera(const std::shared_ptr<camera::CCamera>& LightCamera)
	{
		m_LightCamera = LightCamera;
	}

	const std::shared_ptr<camera::CCamera>& CDrawInfo::GetLightCamera() const
	{
		return m_LightCamera;
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