#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "../../Interface/IPhysicsObject.h"
#include "CBulletRigidBody.h"

namespace physics
{
	class CBulletBox : public IPhysicsObject
	{
		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;
	public:
		CBulletBox();
		virtual ~CBulletBox();

		bool Create(btDiscreteDynamicsWorld* pDynamicsWorld, const glm::vec3& Origin, const glm::vec3& BoxHalfSize, bool IsStatic, float Mass);
	};
}
#endif