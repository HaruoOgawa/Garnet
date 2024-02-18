#pragma once
#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>

namespace physics
{
	class IPhysicsObject;

	class IPhysicsEngine
	{
	public:
		virtual bool Initialize() = 0;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool IsStatic, float Mass) = 0;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(float Radius, bool IsStatic, float Mass) = 0;

		virtual bool Update(float DeltaTime) = 0;
	};
}
#endif // USE_PHYSICS