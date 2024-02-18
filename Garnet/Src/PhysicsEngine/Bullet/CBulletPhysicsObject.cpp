#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletPhysicsObject::CBulletPhysicsObject(bool IsStaticFlag, float Mass):
		m_IsStatic(IsStaticFlag),
		m_Mass(Mass),
		m_WorldScale(glm::vec3(1.0f)),
		m_CollisionShape(nullptr),
		m_RigidBody(nullptr)
	{
	}

	CBulletPhysicsObject::~CBulletPhysicsObject()
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

	bool CBulletPhysicsObject::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		return true;
	}

	bool CBulletPhysicsObject::IsStatic()
	{
		return m_IsStatic;
	}

	glm::mat4 CBulletPhysicsObject::GetCurrentPhysicsWorldMatrix()
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, true, m_WorldScale);

		return WorldMatrix;
	}

	glm::vec3 CBulletPhysicsObject::GetCurrentWorldPos()
	{
		btTransform trans;
		trans.setIdentity();

		if (m_RigidBody)
		{
			trans = m_RigidBody->GetCurrentWorldTransform();
		}

		return glm::vec3(static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
	}

	glm::quat CBulletPhysicsObject::GetCurrentWorldRotate()
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