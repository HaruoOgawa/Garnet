#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"
#include "CBulletPhysicsEngine.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletPhysicsObject::CBulletPhysicsObject(bool IsStaticFlag, float Mass):
		m_IsStatic(IsStaticFlag),
		m_Mass(Mass),
		m_WorldScale(glm::vec3(1.0f)),
		m_CollisionShape(nullptr),
		m_RigidBody(nullptr),
		m_6DofSpringConstraint(nullptr)
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

	const std::shared_ptr<CBulletRigidBody>& CBulletPhysicsObject::GetRigidBody() const
	{
		return m_RigidBody;
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

	void CBulletPhysicsObject::SetPhysicsWorldTransform(const glm::vec3& WorldPos, const glm::quat& WorldRotate, const glm::vec3& WorldScale)
	{
		btTransform trans;
		trans.setIdentity();
		trans.setOrigin(btVector3(WorldPos.x, WorldPos.y, WorldPos.z));
		trans.setRotation(btQuaternion(WorldRotate.x, WorldRotate.y, WorldRotate.z, WorldRotate.w));

		if (m_RigidBody)
		{
			m_RigidBody->SetWorldTransform(trans);
		}
	}

	void CBulletPhysicsObject::Reserve6DofSpringConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject)
	{
		// Constraintを予約しておく
		m_6DofSpringConstraint = std::make_shared<SReservedConstraintData>(FixedObject);
	}

	void CBulletPhysicsObject::Apply6DofSpringConstraint(IPhysicsEngine* pPhysicsEngine)
	{
		if (!m_6DofSpringConstraint) return;

		CBulletPhysicsEngine* pBulletPhysics = static_cast<CBulletPhysicsEngine*>(pPhysicsEngine);

		// ピボットを計算
		// ひとまず２つのRigidBodyの中点とする
		glm::vec3 pointA = GetCurrentWorldPos();
		glm::vec3 pointB = m_6DofSpringConstraint->FixedObject->GetCurrentWorldPos();

		glm::vec3 ConnectPoint = (pointA + pointB) * 0.5f;

		// Constraintsを追加する
		const auto& TargetRigidBody = static_cast<CBulletPhysicsObject*>(m_6DofSpringConstraint->FixedObject.get())->GetRigidBody();
		m_RigidBody->Add6DofSpringConstraint(pBulletPhysics->GetDynamicsWorld(), TargetRigidBody, ConnectPoint);

		// 追加が終わったのでリリースする
		m_6DofSpringConstraint.reset();
		m_6DofSpringConstraint = nullptr;
	}
}
#endif