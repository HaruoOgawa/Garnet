#ifdef USE_PHYSICS
#include "CBulletBox.h"
#include "CBulletPhysicsEngine.h"

namespace physics
{
	CBulletBox::CBulletBox(const glm::vec3& BoxHalfSize, bool Kinematic, float Mass, const SRigidbodyParam& RBParam):
		CBulletPhysicsObject(Kinematic, Mass, RBParam),
		m_BoxHalfSize(BoxHalfSize)
	{
	}

	CBulletBox::~CBulletBox()
	{
	}

	bool CBulletBox::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		CBulletPhysicsEngine* pBulletPhysics = static_cast<CBulletPhysicsEngine*>(pPhysicsEngine);

		m_CollisionShape = std::make_shared<btBoxShape>(btVector3(m_BoxHalfSize.x * WorldScale.x, m_BoxHalfSize.y * WorldScale.y, m_BoxHalfSize.z * WorldScale.z));
		m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), WorldPos, WorldRotate, m_Kinematic, m_Mass, m_RBParam);

		return true;
	}

	glm::vec3 CBulletBox::GetSize()
	{
		return m_BoxHalfSize;
	}
}
#endif