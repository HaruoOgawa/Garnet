#pragma once

#include <glm/glm.hpp>

namespace graphics
{
	class CDrawInfo
	{
		glm::vec4 m_LightDir;
		glm::vec4 m_LightColor;
	public:
		CDrawInfo();
		virtual ~CDrawInfo() = default;

		void SetLightDir(const glm::vec4& LightDir);
		const glm::vec4& GetLightDir() const;

		void SetLightColor(const glm::vec4& LightColor);
		const glm::vec4& GetLightColor() const;
	};
}