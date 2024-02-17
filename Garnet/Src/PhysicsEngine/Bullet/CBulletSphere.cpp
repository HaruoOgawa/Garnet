#ifdef USE_PHYSICS
#include "CBulletSphere.h"

namespace physics
{
	CBulletSphere::CBulletSphere() :
		m_CollisionShape(nullptr),
		m_RigidBody(nullptr)
	{
	}

	CBulletSphere::~CBulletSphere()
	{
		if (m_RigidBody)
		{
			m_RigidBody.reset();
			m_RigidBody = nullptr;
		}

		if (m_CollisionShape)
		{
			m_CollisionShape.reset();
			m_CollisionShape = nullptr;
		}
	}

	bool CBulletSphere::Create(btDiscreteDynamicsWorld* pDynamicsWorld, const glm::vec3& Origin, float Radius, bool IsStatic, float Mass)
	{
		m_CollisionShape = std::make_shared<btSphereShape>(btScalar(Radius));
		m_RigidBody = std::make_shared<CBulletRigidBody>(pDynamicsWorld, m_CollisionShape.get(), Origin, IsStatic, Mass);

		return true;
	}
}
#endif