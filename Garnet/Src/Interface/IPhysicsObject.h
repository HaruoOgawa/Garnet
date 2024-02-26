#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../PhysicsEngine/EJointType.h"
#include "../PhysicsEngine/SJointParam.h"

namespace physics
{
	class IPhysicsEngine;

	class IPhysicsObject
	{
	public:
		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) = 0;

		virtual bool IsStatic() = 0;
		virtual bool IsDynamicJoint() = 0;

		virtual glm::vec3 GetSize() = 0;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() = 0;
		virtual glm::vec3 GetCurrentWorldPos() = 0;
		virtual glm::quat GetCurrentWorldRotate() = 0;

		virtual void SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) = 0;

		virtual void ReserveConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject, EJointType JointType, const SJointParam& JParam) = 0;
		virtual void ApplyConstraint(IPhysicsEngine* pPhysicsEngine, const glm::quat& FixedWorldRotate) = 0;
		virtual void UpdateJointWorldTransform(const glm::vec3& Pos, const glm::quat& Rotate, const glm::vec3& Scale) = 0;
	};
}
#endif