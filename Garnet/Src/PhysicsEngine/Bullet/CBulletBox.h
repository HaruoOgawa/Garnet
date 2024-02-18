#pragma once

#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"

namespace physics
{
	class CBulletBox : public CBulletPhysicsObject
	{
		const glm::vec3 m_BoxHalfSize;
	public:
		CBulletBox(const glm::vec3& BoxHalfSize, bool IsStaticFlag, float Mass);
		virtual ~CBulletBox();

		virtual bool Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale) override;
	};
}
#endif