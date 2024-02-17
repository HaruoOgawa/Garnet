#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

namespace physics
{
	class CBulletRigidBody
	{
		btDiscreteDynamicsWorld* m_pDynamicWorld;

		std::shared_ptr<btDefaultMotionState> m_MotionState;
		std::shared_ptr<btRigidBody> m_Rigidbody;
	private:
		bool Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& Origin, bool IsStatic, float Mass);
	public:
		CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& Origin, bool IsStatic, float Mass);
		virtual ~CBulletRigidBody();

		btTransform GetCurrentWorldTransform();
	};
}
#endif