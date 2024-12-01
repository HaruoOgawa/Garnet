#pragma once

#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"

namespace physics
{
	class CBulletSphere : public CBulletPhysicsObject
	{
		const float m_Radius;
	public:
		CBulletSphere(float Radius, bool Kinematic, const SRigidbodyParam& RBParam);
		virtual ~CBulletSphere();

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual glm::vec3 GetSize() override;
	};
}
#endif