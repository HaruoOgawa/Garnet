#ifdef USE_PHYSICS
#include "CBulletSphere.h"
#include "CBulletPhysics.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletSphere::CBulletSphere(float Radius, bool IsStaticFlag, float Mass) :
		m_Radius(Radius),
		m_IsStatic(IsStaticFlag),
		m_Mass(Mass),
		m_WorldScale(glm::vec3(1.0f)),
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

	bool CBulletSphere::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		m_WorldScale = WorldScale;

		CBulletPhysics* pBulletPhysics = static_cast<CBulletPhysics*>(pPhysicsEngine);

		float MaxScale = fmaxf(WorldScale.x, fmaxf(WorldScale.y, WorldScale.z));

		m_CollisionShape = std::make_shared<btSphereShape>(btScalar(m_Radius * MaxScale));
		m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), WorldPos, WorldRotate, m_IsStatic, m_Mass);

		return true;
	}

	bool CBulletSphere::IsStatic()
	{
		return m_IsStatic;
	}

	glm::mat4 CBulletSphere::GetCurrentPhysicsWorldMatrix()
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, true, m_WorldScale);

		return WorldMatrix;
	}

	glm::vec3 CBulletSphere::GetCurrentWorldPos()
	{
		btTransform trans;
		trans.setIdentity();

		if (m_RigidBody)
		{
			trans = m_RigidBody->GetCurrentWorldTransform();
		}
		
		return glm::vec3(static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
	}

	glm::quat CBulletSphere::GetCurrentWorldRotate()
	{
		btTransform trans;
		trans.setIdentity();

		if (m_RigidBody)
		{
			trans = m_RigidBody->GetCurrentWorldTransform();
		}
		
		return glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
	}
}
#endif