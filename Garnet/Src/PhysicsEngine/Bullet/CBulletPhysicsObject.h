#pragma once

#ifdef USE_PHYSICS
#include "../../Interface/IPhysicsObject.h"
#include <memory>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "CBulletRigidBody.h"

namespace physics
{
	struct SReservedConstraintData
	{
		std::shared_ptr<IPhysicsObject> FixedObject = nullptr;
		EJointType JointType = EJointType::NONE;
		SJointParam JParam = {};

		SReservedConstraintData(const std::shared_ptr<IPhysicsObject>& object, EJointType t, const SJointParam& param) :
			FixedObject(object),
			JointType(t),
			JParam(param)
		{
		}
	};

	class CBulletPhysicsObject : public IPhysicsObject
	{
	protected:
		const bool m_IsStatic;
		const float m_Mass;
		const SRigidbodyParam m_RBParam;

		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;

		// Constraint
		std::vector<std::shared_ptr<SReservedConstraintData>> m_ReservedConstraintList;
	public:
		CBulletPhysicsObject(bool IsStaticFlag, float Mass, const SRigidbodyParam& RBParam);
		virtual ~CBulletPhysicsObject();

		const std::shared_ptr<CBulletRigidBody>& GetRigidBody() const;

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual bool IsStatic() override;
		virtual bool IsDynamicJoint() override;

		virtual glm::vec3 GetSize() override;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;

		virtual void SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual void ReserveConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject, EJointType JointType, const SJointParam& JParam) override;
		virtual void ApplyConstraint(IPhysicsEngine* pPhysicsEngine, const glm::quat& FixedWorldRotate) override;
		virtual void UpdateJointWorldTransform(const glm::vec3& Pos, const glm::quat& Rotate, const glm::vec3& Scale) override;
	};
}
#endif