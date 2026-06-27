#include "CDrawInfo.h"

namespace graphics
{
	CDrawInfo::CDrawInfo() :
		m_CurrentFrame(0),
		m_LightCamera(std::make_shared<camera::CCamera>()),
		m_LightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
		m_LightProjection(std::make_shared<projection::CProjection>()),
		m_SecondsTime(0.0f),
		m_DeltaSecondsTime(0.0f),
		m_SpatialCulling(false),
		m_SpatialCullPos(glm::vec4(0.0f))
	{
	}

	void CDrawInfo::DoNextFrame()
	{
		if (m_CurrentFrame == INT_MAX)
		{
			m_CurrentFrame = 0;
		}
		else
		{
			m_CurrentFrame++;
		}
	}

	int CDrawInfo::GetCurrentFrame() const
	{
		return m_CurrentFrame;
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

	void  CDrawInfo::SetSpatialCulling(bool Flag)
	{
		m_SpatialCulling = Flag;
	}

	bool  CDrawInfo::IsSpatialCulling() const
	{
		return m_SpatialCulling;
	}

	void CDrawInfo::SetSpatialCullPos(const glm::vec4& Pos)
	{
		m_SpatialCullPos = Pos;
	}

	const glm::vec4& CDrawInfo::GetSpatialCullPos() const
	{
		return m_SpatialCullPos;
	}
}