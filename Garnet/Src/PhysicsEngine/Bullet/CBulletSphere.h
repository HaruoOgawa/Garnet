#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "../../Interface/IPhysicsObject.h"
#include "CBulletRigidBody.h"

namespace physics
{
	class CBulletSphere : public IPhysicsObject
	{
		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;
	public:
		CBulletSphere();
		virtual ~CBulletSphere();

		bool Create(btDiscreteDynamicsWorld* pDynamicsWorld, const glm::vec3& Origin, float Radius, bool IsStatic, float Mass);
	};
}
#endif