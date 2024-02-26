#ifdef USE_PHYSICS
#include "CBulletPhysicsObject.h"
#include "CBulletPhysicsEngine.h"
#include "../../Math/CTransform.h"

namespace physics
{
	CBulletPhysicsObject::CBulletPhysicsObject(bool IsStaticFlag, float Mass, const SRigidbodyParam& RBParam):
		m_IsStatic(IsStaticFlag),
		m_Mass(Mass),
		m_RBParam(RBParam),
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

	bool CBulletPhysicsObject::IsDynamicJoint()
	{
		return (m_RBParam.PhysicsType == EPhysicsType::DYNAMIC_JOINT);
	}

	glm::vec3 CBulletPhysicsObject::GetSize()
	{
		return glm::vec3(1.0f);
	}

	glm::mat4 CBulletPhysicsObject::GetCurrentPhysicsWorldMatrix()
	{
		glm::vec3 Pos = GetCurrentWorldPos();
		glm::quat Rot = GetCurrentWorldRotate();

		glm::mat4 WorldMatrix = glm::mat4(1.0f);
		math::CTransform::CalcModelMatrix(WorldMatrix, Pos, Rot, false);

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

	void CBulletPhysicsObject::ReserveConstraint(const std::shared_ptr<IPhysicsObject>& FixedObject, EJointType JointType, const SJointParam& JParam)
	{
		// Constraint‚ð—\–ñ‚µ‚Ä‚¨‚­
		m_ReservedConstraintList.push_back(std::make_shared<SReservedConstraintData>(FixedObject, JointType, JParam));
	}

	void CBulletPhysicsObject::ApplyConstraint(IPhysicsEngine* pPhysicsEngine, const glm::quat& FixedWorldRotate)
	{
		CBulletPhysicsEngine* pBulletPhysics = static_cast<CBulletPhysicsEngine*>(pPhysicsEngine);

		for (const auto& ReservedConstraint : m_ReservedConstraintList)
		{
			if (ReservedConstraint->JointType == EJointType::SPRING_6DOF)
			{
				// Constraints‚ð’Ç‰Á‚·‚é
				const auto& TargetRigidBody = static_cast<CBulletPhysicsObject*>(ReservedConstraint->FixedObject.get())->GetRigidBody();
				m_RigidBody->Add6DofSpringConstraint(pBulletPhysics->GetDynamicsWorld(), TargetRigidBody, ReservedConstraint->JParam, FixedWorldRotate);
			}
			else if (ReservedConstraint->JointType == EJointType::Generic_6DOF)
			{
				// –¢ŽÀ‘•
			}
			else if (ReservedConstraint->JointType == EJointType::P2P)
			{
				// –¢ŽÀ‘•
			}
			else if (ReservedConstraint->JointType == EJointType::ConeTwist)
			{
				// –¢ŽÀ‘•
			}
			else if (ReservedConstraint->JointType == EJointType::Slider)
			{
				// –¢ŽÀ‘•
			}
		}

		// ’Ç‰Á‚ªI‚í‚Á‚½‚Ì‚ÅƒŠƒŠ[ƒX‚·‚é
		m_ReservedConstraintList.clear();
	}

	void CBulletPhysicsObject::UpdateJointWorldTransform(const glm::vec3& Pos, const glm::quat& Rotate, const glm::vec3& Scale)
	{
		if (!m_RigidBody) return;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(Pos.x, Pos.y, Pos.z));
		transform.setRotation(btQuaternion(Rotate.x, Rotate.y, Rotate.z, Rotate.w));

		m_RigidBody->UpdateJointWorldTransform(transform);
	}
}
#endif