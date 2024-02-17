#ifdef USE_PHYSICS
#include "CBulletBox.h"

namespace physics
{
	CBulletBox::CBulletBox():
		m_CollisionShape(nullptr),
		m_RigidBody(nullptr)
	{
	}

	CBulletBox::~CBulletBox()
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

	bool CBulletBox::Create(btDiscreteDynamicsWorld* pDynamicsWorld, const glm::vec3& Origin, const glm::vec3& BoxHalfSize, bool IsStatic, float Mass)
	{
		m_CollisionShape = std::make_shared<btBoxShape>(btVector3(BoxHalfSize.x, BoxHalfSize.y, BoxHalfSize.z));
		m_RigidBody = std::make_shared<CBulletRigidBody>(pDynamicsWorld, m_CollisionShape.get(), Origin, IsStatic, Mass);

		return true;
	}
}
#endif