#include "CDrawInfo.h"

namespace graphics
{
	CDrawInfo::CDrawInfo() :
		m_LightCamera(std::make_shared<camera::CCamera>()),
		m_LightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
		m_LightProjection(std::make_shared<projection::CProjection>()),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f)
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

	void CDrawInfo::SetSecondsTime(float SecondsTime)
	{
		m_SecondsTime = SecondsTime;
	}

	float CDrawInfo::GetSecondsTime() const
	{
		return m_SecondsTime;
	}
	
	void CDrawInfo::SetDeltaSecondsTime(float DeltaSecondsTime)
	{
		m_DeltaSecondsTime = DeltaSecondsTime;
	}

	float CDrawInfo::GetDeltaSecondsTime() const
	{
		return m_DeltaSecondsTime;
	}
}