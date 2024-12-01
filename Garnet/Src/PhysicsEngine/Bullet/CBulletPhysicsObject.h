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
	class CBulletPhysicsObject : public IPhysicsObject
	{
	protected:
		const bool m_Kinematic;
		const SRigidbodyParam m_RBParam;

		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;

		const EPhysicsShape m_PhysicsShape;

		// Constraint
		std::vector<std::shared_ptr<SConstraintData>> m_ConstraintList;
	public:
		CBulletPhysicsObject(bool Kinematic, const SRigidbodyParam& RBParam, EPhysicsShape PhysicsShape);
		virtual ~CBulletPhysicsObject();

		const std::shared_ptr<CBulletRigidBody>& GetRigidBody() const;

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual bool IsStatic() override;
		virtual bool IsKinematic() override;
		virtual bool IsDynamicJoint() override;

		virtual glm::vec3 GetSize() override;

		virtual const std::vector<std::shared_ptr<SConstraintData>>& GetConstraintList() const override;

		virtual EPhysicsShape GetPhysicsShape() const override;

		virtual const SRigidbodyParam& GetRbParam() const override;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;

		virtual void SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate) override;

		virtual void ReserveConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject, EJointType JointType, const SJointParam& JParam) override;
		virtual void ApplyConstraint(IPhysicsEngine* pPhysicsEngine) override;
		virtual void AlignConstraint(const glm::vec3& WorldPos, const glm::quat& WorldRotate) override;

		virtual void SetActivation(bool Flag) override;
	};
}
#endif