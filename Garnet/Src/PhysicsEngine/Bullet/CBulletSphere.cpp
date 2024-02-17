#ifdef USE_PHYSICS
#include "CBulletSphere.h"
#include "../../Math/CTransform.h"

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

	glm::mat4 CBulletSphere::GetCurrentPhysicsWorldMatrix(const glm::vec3& Scale)
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, true, Scale);

		return WorldMatrix;
	}

	glm::vec3 CBulletSphere::GetCurrentWorldPos()
	{
		btTransform trans = m_RigidBody->GetCurrentWorldTransform();
		
		return glm::vec3(static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
	}

	glm::quat CBulletSphere::GetCurrentWorldRotate()
	{
		auto trans = m_RigidBody->GetCurrentWorldTransform();
		
		return glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
	}
}
#endif