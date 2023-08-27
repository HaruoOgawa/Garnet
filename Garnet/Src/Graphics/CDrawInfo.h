#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace graphics
{
	class CDrawInfo
	{
		std::shared_ptr<camera::CCamera> m_LightCamera;
		glm::vec4 m_LightColor;
		std::shared_ptr<projection::CProjection> m_LightProjection;

		float m_SecondsTime;
		float m_DeltaSecondsTime;
	public:
		CDrawInfo();
		virtual ~CDrawInfo() = default;

		void SetLightCamera(const std::shared_ptr<camera::CCamera>& LightCamera);
		const std::shared_ptr<camera::CCamera>& GetLightCamera() const;

		void SetLightColor(const glm::vec4& LightColor);
		const glm::vec4& GetLightColor() const;

		const std::shared_ptr<projection::CProjection>& GetLightProjection() const { return m_LightProjection; };

		void SetSecondsTime(float SecondsTime);
		float GetSecondsTime() const;
		
		void SetDeltaSecondsTime(float DeltaSecondsTime);
		float GetDeltaSecondsTime() const;
	};
}