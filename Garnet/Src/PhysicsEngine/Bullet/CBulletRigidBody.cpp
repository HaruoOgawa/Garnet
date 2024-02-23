#ifdef USE_PHYSICS
#include "CBulletRigidBody.h"

namespace physics
{
	CBulletRigidBody::CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsStatic, float Mass, const SRigidbodyParam& RBParam):
		m_pDynamicWorld(pDynamicWorld),
		m_MotionState(nullptr),
		m_Rigidbody(nullptr)
	{
		Create(pDynamicWorld, pCollisionShape, WorldPos, WorldRotate, IsStatic, Mass, RBParam);
	}

	CBulletRigidBody::~CBulletRigidBody()
	{
		if (m_MotionState)
		{
			m_MotionState.reset();
			m_MotionState = nullptr;
		}

		if (m_Rigidbody)
		{
			m_pDynamicWorld->removeCollisionObject(m_Rigidbody.get());

			m_Rigidbody.reset();
			m_Rigidbody = nullptr;
		}
	}

	bool CBulletRigidBody::Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsStatic, float Mass, const SRigidbodyParam& RBParam)
	{
		// Transform
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(WorldPos.x, WorldPos.y, WorldPos.z));
		transform.setRotation(btQuaternion(WorldRotate.x, WorldRotate.y, WorldRotate.z, WorldRotate.w));

		// 質量
		btScalar bodyMass(((IsStatic)? 0.0f : Mass));

		// Bulletは質量が0のものはStatic(固定されている)、そうでないものはDynamic(物理演算で動く)として扱われる
		bool IsDynamic = (bodyMass != 0.0f);

		// Inertiaは慣性の意味
		btVector3 localInertia(0, 0, 0);
		// 慣性力の計算でここではそれを0に初期化している
		if (IsDynamic)
		{
			pCollisionShape->calculateLocalInertia(bodyMass, localInertia);
		}

		// MotoinState. 補間だったり他のアクティブオブジェクトとの同期に使用される
		// staticオブジェクトだとMotioStateのTransformは更新されないのでnullptrにしておく必要がある(代わりにRigidBodyの方が更新される)
		// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12634
		btDefaultMotionState* pMotionState = nullptr;
		if (IsDynamic)
		{
			m_MotionState = std::make_shared<btDefaultMotionState>(transform);

			pMotionState = m_MotionState.get();
		}

		// RigidBodyの設定. 物理演算に使用するオブジェクト.物理演算に関するパラメーターを持っている
		btRigidBody::btRigidBodyConstructionInfo rbInfo(bodyMass, pMotionState, pCollisionShape, localInertia);
		m_Rigidbody = std::make_shared<btRigidBody>(rbInfo);

		m_Rigidbody->setDamping(RBParam.TransDamping, RBParam.RotateDamping);
		m_Rigidbody->setRestitution(RBParam.Repulsion); // 反発係数の設定
		m_Rigidbody->setFriction(RBParam.Friction); // 摩擦係数の設定

		// RigidBodyを物理演算ワールドに追加
		pDynamicWorld->addRigidBody(m_Rigidbody.get());

		return true;
	}

	void CBulletRigidBody::Add6DofSpringConstraint(btDiscreteDynamicsWorld* pDynamicWorld, const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam)
	{
		// Constraintsを追加
		// btGeneric6DofSpring2Constraint(*d6body0,*fixedBody1,frameInA,frameInB);
		// frameInAとframeInBはバネに例えるとバネの端点・剛体との接合点を表す. 二つの剛体にバネを挟むことをイメージするとわかりやすい. それは必ず２つの接合点があるはずである
		// frameInA => d6body0の接合点
		// frameInB => fixedBody1の接合点
		btGeneric6DofSpring2Constraint* spring = new btGeneric6DofSpring2Constraint(
			*m_Rigidbody.get(), 
			*FixedRigidbody->GetbtRigidBody().get(),
			btTransform(btQuaternion::getIdentity(), { JParam.Pos.x, JParam.Pos.y, JParam.Pos.z }),
			btTransform(btQuaternion::getIdentity(), { JParam.Rotate.x, JParam.Rotate.y, JParam.Rotate.z })
		);

		// 関数名の通り移動できる範囲・回転できる範囲
		spring->setLinearLowerLimit(btVector3(JParam.LowerTransLimit.x, JParam.LowerTransLimit.y, JParam.LowerTransLimit.z));
		spring->setLinearUpperLimit(btVector3(JParam.UpperTransLimit.x, JParam.UpperTransLimit.y, JParam.UpperTransLimit.z));
		spring->setAngularLowerLimit(btVector3(JParam.LowerRotateLimit.x, JParam.LowerRotateLimit.y, JParam.LowerRotateLimit.z));
		spring->setAngularUpperLimit(btVector3(JParam.UpperRotateLimit.x, JParam.UpperRotateLimit.y, JParam.UpperRotateLimit.z));

		spring->enableSpring(1, true);
		spring->setStiffness(1, JParam.TransSpring.x); // Stiffness: 硬さ
		spring->setDamping(1, 0.5f); // Damping: 減衰力

		pDynamicWorld->addConstraint(spring, false);
	}

	btTransform CBulletRigidBody::GetCurrentWorldTransform()
	{
		btTransform trans;

		if (m_Rigidbody)
		{
			if (m_MotionState)
			{
				m_MotionState->getWorldTransform(trans);
			}
			else
			{
				trans = m_Rigidbody->getWorldTransform();
			}
		}

		return trans;
	}

	void CBulletRigidBody::SetWorldTransform(const btTransform& trans)
	{
		if (m_Rigidbody)
		{
			if (m_MotionState)
			{
				m_MotionState->setWorldTransform(trans);
			}
			else
			{
				m_Rigidbody->setWorldTransform(trans);
			}
		}
	}

	const std::shared_ptr<btRigidBody>& CBulletRigidBody::GetbtRigidBody() const
	{
		return m_Rigidbody;
	}
}
#endif