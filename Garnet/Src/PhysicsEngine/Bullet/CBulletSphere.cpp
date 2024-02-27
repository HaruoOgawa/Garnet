#ifdef USE_PHYSICS
#include "CBulletSphere.h"
#include "CBulletPhysicsEngine.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletSphere::CBulletSphere(float Radius, bool IsStaticFlag, float Mass, const SRigidbodyParam& RBParam) :
		CBulletPhysicsObject(IsStaticFlag, Mass, RBParam),
		m_Radius(Radius)
	{
	}

	CBulletSphere::~CBulletSphere()
	{
	}

	bool CBulletSphere::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		CBulletPhysicsEngine* pBulletPhysics = static_cast<CBulletPhysicsEngine*>(pPhysicsEngine);

		if (m_RBParam.UseSelfInitialTransform)
		{
			m_CollisionShape = std::make_shared<btSphereShape>(btScalar(m_Radius));
			m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), m_RBParam.InitWorldPos, m_RBParam.InitWorldRotate, m_IsStatic, m_Mass, m_RBParam);
		}
		else
		{
			float MaxScale = fmaxf(WorldScale.x, fmaxf(WorldScale.y, WorldScale.z));

			m_CollisionShape = std::make_shared<btSphereShape>(btScalar(m_Radius * MaxScale));
			m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), WorldPos, WorldRotate, m_IsStatic, m_Mass, m_RBParam);
		}

		return true;
	}

	glm::vec3 CBulletSphere::GetSize()
	{
		return glm::vec3(m_Radius);
	}
}
#endif