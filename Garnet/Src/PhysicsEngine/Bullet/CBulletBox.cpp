#ifdef USE_PHYSICS
#include "CBulletBox.h"
#include "../../Math/CTransform.h"

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

	glm::mat4 CBulletBox::GetCurrentPhysicsWorldMatrix(const glm::vec3& Scale)
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, true, Scale);

		return WorldMatrix;
	}

	glm::vec3 CBulletBox::GetCurrentWorldPos()
	{
		btTransform trans = m_RigidBody->GetCurrentWorldTransform();

		return glm::vec3(static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
	}

	glm::quat CBulletBox::GetCurrentWorldRotate()
	{
		auto trans = m_RigidBody->GetCurrentWorldTransform();

		return glm::quat(trans.getRotation().getW(), trans.getRotation().getX(), trans.getRotation().getY(), trans.getRotation().getZ());
	}
}
#endif