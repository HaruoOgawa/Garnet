#ifdef USE_PHYSICS
#include "CBulletCapsule.h"
#include "CBulletPhysicsEngine.h"

namespace physics
{
	CBulletCapsule::CBulletCapsule(float Radius, float Height, bool Kinematic, float Mass, const SRigidbodyParam& RBParam) :
		CBulletPhysicsObject(Kinematic, Mass, RBParam),
		m_Radius(Radius),
		m_Height(Height)
	{
	}

	CBulletCapsule::~CBulletCapsule()
	{
	}

	bool CBulletCapsule::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		CBulletPhysicsEngine* pBulletPhysics = static_cast<CBulletPhysicsEngine*>(pPhysicsEngine);

		m_CollisionShape = std::make_shared<btCapsuleShape>(m_Radius * WorldScale.x, m_Height * WorldScale.y);
		m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), WorldPos, WorldRotate, m_Kinematic, m_Mass, m_RBParam);

		return true;
	}

	glm::vec3 CBulletCapsule::GetSize()
	{
		return glm::vec3(m_Radius, m_Height, m_Radius);
	}
}
#endif