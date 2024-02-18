#ifdef USE_PHYSICS
#include "CBulletBox.h"
#include "CBulletPhysics.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletBox::CBulletBox(const glm::vec3& BoxHalfSize, bool IsStaticFlag, float Mass):
		m_BoxHalfSize(BoxHalfSize),
		m_IsStatic(IsStaticFlag),
		m_Mass(Mass),
		m_WorldScale(glm::vec3(1.0f)),
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

	bool CBulletBox::Create(IPhysicsEngine* pPhysicsEngine, const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		m_WorldScale = WorldScale;

		CBulletPhysics* pBulletPhysics = static_cast<CBulletPhysics*>(pPhysicsEngine);

		m_CollisionShape = std::make_shared<btBoxShape>(btVector3(m_BoxHalfSize.x * WorldScale.x, m_BoxHalfSize.y * WorldScale.y, m_BoxHalfSize.z * WorldScale.z));
		m_RigidBody = std::make_shared<CBulletRigidBody>(pBulletPhysics->GetDynamicsWorld(), m_CollisionShape.get(), WorldPos, WorldRotate, m_IsStatic, m_Mass);

		return true;
	}

	bool CBulletBox::IsStatic()
	{
		return m_IsStatic;
	}

	glm::mat4 CBulletBox::GetCurrentPhysicsWorldMatrix()
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, true, m_WorldScale);

		return WorldMatrix;
	}

	glm::vec3 CBulletBox::GetCurrentWorldPos()
	{
		btTransform trans;
		trans.setIdentity();

		if (m_RigidBody)
		{
			trans = m_RigidBody->GetCurrentWorldTransform();
		}

		return glm::vec3(static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
	}

	glm::quat CBulletBox::GetCurrentWorldRotate()
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