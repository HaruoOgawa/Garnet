#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace camera { class CCamera; }

namespace graphics
{
	class CDrawInfo
	{
		std::shared_ptr<camera::CCamera> m_LightCamera;
		glm::vec4 m_LightColor;
	public:
		CDrawInfo();
		virtual ~CDrawInfo() = default;

		void SetLightCamera(const std::shared_ptr<camera::CCamera>& LightCamera);
		const std::shared_ptr<camera::CCamera>& GetLightCamera() const;

		void SetLightColor(const glm::vec4& LightColor);
		const glm::vec4& GetLightColor() const;
	};
}