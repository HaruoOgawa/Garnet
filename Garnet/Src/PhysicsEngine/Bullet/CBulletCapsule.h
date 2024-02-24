#pragma once

#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"

namespace physics
{
	class CBulletCapsule : public CBulletPhysicsObject
	{
		const float m_Radius;
		const float m_Height;
	public:
		CBulletCapsule(float Radius, float Height, bool IsStaticFlag, float Mass, const SRigidbodyParam& RBParam);
		virtual ~CBulletCapsule();

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;

		virtual glm::vec3 GetSize() override;
	};
}
#endif