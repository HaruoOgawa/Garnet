#pragma once
#ifdef USE_PHYSICS
#include <memory>
#include "IPhysicsObject.h"

namespace physics
{
	class IPhysicsEngine
	{
	public:
		virtual bool Initialize() = 0;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& Origin, const glm::vec3& BoxHalfSize, bool IsStatic, float Mass) = 0;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(const glm::vec3& Origin, float Radius, bool IsStatic, float Mass) = 0;
	};
}
#endif // USE_PHYSICS