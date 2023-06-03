#include "CObject.h"

namespace object
{
	bool CObject::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const glm::vec4& LightDir)
	{
		return true;
	}

	bool CObject::Draw()
	{
		return true;
	}
}