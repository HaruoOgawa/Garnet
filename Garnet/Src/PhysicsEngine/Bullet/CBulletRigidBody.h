#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

namespace physics
{
	class CBulletRigidBody
	{
		btDiscreteDynamicsWorld* m_pDynamicWorld;

		std::shared_ptr<btDefaultMotionState> m_MotionState;
		std::shared_ptr<btRigidBody> m_Rigidbody;
	private:
		bool Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsStatic, float Mass);
	public:
		CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsStatic, float Mass);
		virtual ~CBulletRigidBody();

		void AddSpringConstraint(btDiscreteDynamicsWorld* pDynamicWorld, const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, const glm::vec3& ConnectPoint);

		btTransform GetCurrentWorldTransform();

		const std::shared_ptr<btRigidBody>& GetbtRigidBody() const;
	};
}
#endif