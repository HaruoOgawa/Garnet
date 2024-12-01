#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "../PhysicsEngine/SRigidbodyParam.h"

namespace physics
{
	class IPhysicsObject;

	class IPhysicsEngine
	{
	public:
		virtual bool IsEnabled() const = 0;
		virtual void SetEnabled(bool Flag) = 0;

		virtual bool Initialize() = 0;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool IsStatic, const SRigidbodyParam& RBParam) = 0;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(float Radius, bool IsStatic, const SRigidbodyParam& RBParam) = 0;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsCapsule(float Radius, float Height, bool IsStatic, const SRigidbodyParam& RBParam) = 0;

		virtual bool Update(float DeltaTime) = 0;
	};
}
