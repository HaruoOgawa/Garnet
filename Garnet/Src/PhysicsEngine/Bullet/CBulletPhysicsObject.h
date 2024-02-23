#pragma once

#ifdef USE_PHYSICS
#include "../../Interface/IPhysicsObject.h"
#include <memory>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "CBulletRigidBody.h"

namespace physics
{
	struct SReservedConstraintData
	{
		std::shared_ptr<IPhysicsObject> FixedObject = nullptr;

		SReservedConstraintData(const std::shared_ptr<IPhysicsObject>& object) :
			FixedObject(object)
		{
		}
	};

	class CBulletPhysicsObject : public IPhysicsObject
	{
	protected:
		const bool m_IsStatic;
		const float m_Mass;

		glm::vec3 m_WorldScale;

		std::shared_ptr<btCollisionShape> m_CollisionShape;
		std::shared_ptr<CBulletRigidBody> m_RigidBody;

		// Constraint
		std::shared_ptr<SReservedConstraintData> m_6DofSpringConstraint;
	public:
		CBulletPhysicsObject(bool IsStaticFlag, float Mass);
		virtual ~CBulletPhysicsObject();

		const std::shared_ptr<CBulletRigidBody>& GetRigidBody() const;

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual bool IsStatic() override;

		virtual glm::mat4 GetCurrentPhysicsWorldMatrix() override;
		virtual glm::vec3 GetCurrentWorldPos() override;
		virtual glm::quat GetCurrentWorldRotate() override;

		virtual void SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual void Reserve6DofSpringConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject) override;

		virtual void Apply6DofSpringConstraint(IPhysicsEngine* pPhysicsEngine) override;
	};
}
#endif