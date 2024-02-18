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
		const glm::vec3 m_BoxHalfSize;
		const bool m_IsStatic;
		const float m_Mass;

		glm::vec3 m_WorldScale;

		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;
	public:
		CBulletBox(const glm::vec3& BoxHalfSize, bool IsStaticFlag, float Mass);
		virtual ~CBulletBox();

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual bool IsStatic() override;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;
	};
}
#endif