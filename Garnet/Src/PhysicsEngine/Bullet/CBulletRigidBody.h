#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

#include "../EJointType.h"
#include "../SRigidbodyParam.h"
#include "../SJointParam.h"

namespace physics
{
	class CBulletRigidBody
	{
		const SRigidbodyParam m_RBParam;

		btDiscreteDynamicsWorld* m_pDynamicWorld;

		std::shared_ptr<btDefaultMotionState> m_MotionState;
		std::shared_ptr<btRigidBody> m_Rigidbody;

		EJointType m_JointType;
		std::vector<std::shared_ptr<btGeneric6DofSpringConstraint>> m_6DofSpringConstraintList;
	private:
		bool Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsKinematic, float Mass, const SRigidbodyParam& RBParam);
	public:
		CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsKinematic, float Mass, const SRigidbodyParam& RBParam);
		virtual ~CBulletRigidBody();

		const SRigidbodyParam& GetRbParam() const;

		void Add6DofSpringConstraint(btDiscreteDynamicsWorld* pDynamicWorld, const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam);
		void AlignConstraint(const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam, const btTransform& JointWorldTransform);

		btTransform GetCurrentWorldTransform();
		void SetWorldTransform(const btTransform& trans);

		const std::shared_ptr<btRigidBody>& GetbtRigidBody() const;
	};
}
#endif