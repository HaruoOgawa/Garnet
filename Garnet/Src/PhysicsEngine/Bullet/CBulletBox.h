#pragma once

#ifdef USE_PHYSICS
#include <memory>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

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

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix(const glm::vec3& Scale) override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;
	};
}
#endif