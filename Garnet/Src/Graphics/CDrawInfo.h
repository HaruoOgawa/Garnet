#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"

namespace graphics
{
	class CDrawInfo
	{
		int m_CurrentFrame;

		std::shared_ptr<camera::CCamera> m_LightCamera;
		glm::vec4 m_LightColor;
		std::shared_ptr<projection::CProjection> m_LightProjection;

		float m_SecondsTime;
		float m_DeltaSecondsTime;

		bool  m_SpatialCulling;
		glm::vec4 m_SpatialCullPos;
	public:
		CDrawInfo();
		virtual ~CDrawInfo() = default;

		void DoNextFrame();
		int GetCurrentFrame() const;

		void SetLightCamera(const std::shared_ptr<camera::CCamera>& LightCamera);
		const std::shared_ptr<camera::CCamera>& GetLightCamera() const;

		void SetLightColor(const glm::vec4& LightColor);
		const glm::vec4& GetLightColor() const;

		const std::shared_ptr<projection::CProjection>& GetLightProjection() const { return m_LightProjection; };

		void SetSecondsTime(float SecondsTime);
		float GetSecondsTime() const;
		
		void SetDeltaSecondsTime(float DeltaSecondsTime);
		float GetDeltaSecondsTime() const;

		void  SetSpatialCulling(bool Flag);
		bool  IsSpatialCulling() const;

		void SetSpatialCullPos(const glm::vec4& Pos);
		const glm::vec4& GetSpatialCullPos() const;
	};
}