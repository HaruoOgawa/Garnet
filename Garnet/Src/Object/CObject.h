#pragma once

#include <memory>

// óeó Ç™Ç©Ç≥ÇﬁÇÃÇ≈ç≈è¨å¿ÇÃé¿ëïÇ≈
namespace camera { class CCamera; }
namespace projection { class CProjection; }
#include <glm/glm.hpp>

namespace object
{
	class CObject
	{
	public:
		CObject() = default;
		virtual ~CObject() = default;

		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const glm::vec4& LightDir);
		virtual bool Draw();
	};
}