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
	struct SConstraintData
	{
		std::shared_ptr<IPhysicsObject> FixedObject = nullptr;
		EJointType JointType = EJointType::NONE;
		SJointParam JParam = {};

		SConstraintData(const std::shared_ptr<IPhysicsObject>& object, EJointType t, const SJointParam& param) :
			FixedObject(object),
			JointType(t),
			JParam(param)
		{
		}
	};

	class CBulletPhysicsObject : public IPhysicsObject
	{
	protected:
		const bool m_Kinematic;
		const float m_Mass;
		const SRigidbodyParam m_RBParam;

		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;

		// Constraint
		std::vector<std::shared_ptr<SConstraintData>> m_ConstraintList;
	public:
		CBulletPhysicsObject(bool Kinematic, float Mass, const SRigidbodyParam& RBParam);
		virtual ~CBulletPhysicsObject();

		const std::shared_ptr<CBulletRigidBody>& GetRigidBody() const;

		const std::vector<std::shared_ptr<SConstraintData>>& GetConstraintList() const;

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual bool IsStatic() override;
		virtual bool IsKinematic() override;
		virtual bool IsDynamicJoint() override;

		virtual glm::vec3 GetSize() override;

		virtual const SRigidbodyParam& GetRbParam() const override;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;

		virtual void SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate) override;

		virtual void ReserveConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject, EJointType JointType, const SJointParam& JParam) override;
		virtual void ApplyConstraint(IPhysicsEngine* pPhysicsEngine) override;
		virtual void AlignConstraint(const glm::vec3& WorldPos, const glm::quat& WorldRotate) override;
	};
}
#endif