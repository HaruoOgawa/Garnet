#ifdef USE_PHYSICS
#include "CBulletRigidBody.h"

namespace physics
{
	CBulletRigidBody::CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsStatic, float Mass, const SRigidbodyParam& RBParam):
		m_pDynamicWorld(pDynamicWorld),
		m_MotionState(nullptr),
		m_Rigidbody(nullptr),
		m_JointType(EJointType::NONE)
	{
		Create(pDynamicWorld, pCollisionShape, WorldPos, WorldRotate, IsStatic, Mass, RBParam);
	}

	CBulletRigidBody::~CBulletRigidBody()
	{
		for (const auto& Constraint : m_6DofSpringConstraintList)
		{
			if (Constraint)
			{
				m_pDynamicWorld->removeConstraint(Constraint.get());
			}
		}
		
		m_6DofSpringConstraintList.clear();

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
		// 非衝突グループの設定のビットマスクもここで設定する
		pDynamicWorld->addRigidBody(m_Rigidbody.get(), RBParam.group, ~RBParam.NoneCollideGroupFlag);

		return true;
	}

	void CBulletRigidBody::Add6DofSpringConstraint(btDiscreteDynamicsWorld* pDynamicWorld, const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam, const glm::quat& FixedWorldRotate)
	{
		m_JointType = EJointType::SPRING_6DOF;

		// Constraintsを追加
		// btGeneric6DofSpring2Constraint(*d6body0,*fixedBody1,frameInA,frameInB);
		// frameInAとframeInBはバネに例えるとバネの端点・剛体との接合点を表す. 二つの剛体にバネを挟むことをイメージするとわかりやすい. それは必ず２つの接合点があるはずである
		// frameInAはd6body0の接合点、frameInBのfixedBody1の接合点
		// そしてその座標はframeInA・frameInBともに『『fixedBody1』』の座標を中心とした移動・回転で表される

		btQuaternion RotateA = btQuaternion(JParam.Rotate6DofBody.x, JParam.Rotate6DofBody.y, JParam.Rotate6DofBody.z, JParam.Rotate6DofBody.w);
		btQuaternion FixedRotateB = btQuaternion(FixedWorldRotate.x, FixedWorldRotate.y, FixedWorldRotate.z, FixedWorldRotate.w);

		std::shared_ptr< btGeneric6DofSpring2Constraint> Constraint = std::make_shared<btGeneric6DofSpring2Constraint>(
			*m_Rigidbody.get(), 
			*FixedRigidbody->GetbtRigidBody().get(),
			btTransform(RotateA, { JParam.Pos6DofBody.x, JParam.Pos6DofBody.y, JParam.Pos6DofBody.z }),
			btTransform(FixedRotateB, { 0.0f, 0.0f, 0.0f })
		);

		// 関数名の通り移動できる範囲・回転できる範囲を設定
		{
			Constraint->setLinearLowerLimit(btVector3(JParam.LowerTransLimit.x, JParam.LowerTransLimit.y, JParam.LowerTransLimit.z));
			Constraint->setLinearUpperLimit(btVector3(JParam.UpperTransLimit.x, JParam.UpperTransLimit.y, JParam.UpperTransLimit.z));
			Constraint->setAngularLowerLimit(btVector3(JParam.LowerRotateLimit.x, JParam.LowerRotateLimit.y, JParam.LowerRotateLimit.z));
			Constraint->setAngularUpperLimit(btVector3(JParam.UpperRotateLimit.x, JParam.UpperRotateLimit.y, JParam.UpperRotateLimit.z));
		}

		// 軸単位のパラメーターを設定
		for(int a = 0; a < 3; a++)
		{
			Constraint->enableSpring(a, true);
			Constraint->setStiffness(a, 1.0f); // Stiffness: 硬さ
			Constraint->setDamping(a, 1.0f); // Damping: 減衰力
		}

		// 現在の位置をバネの釣り合いの位置(自然長)にする
		Constraint->setEquilibriumPoint();

		// 物理ワールドに追加
		pDynamicWorld->addConstraint(Constraint.get(), false);

		//
		m_6DofSpringConstraintList.push_back(Constraint);
	}

	void CBulletRigidBody::UpdateJointWorldTransform(const btTransform& transform)
	{
		if (m_JointType == EJointType::SPRING_6DOF)
		{
			for (const auto& Constraint : m_6DofSpringConstraintList)
			{
				if (Constraint)
				{
					Constraint->setFrames(
						transform,
						btTransform(btQuaternion::getIdentity(), { 0.0f, 0.0f, 0.0f })
					);
				}
			}
		}
		else if (m_JointType == EJointType::Generic_6DOF)
		{
			// 未実装
		}
		else if (m_JointType == EJointType::P2P)
		{
			// 未実装
		}
		else if (m_JointType == EJointType::ConeTwist)
		{
			// 未実装
		}
		else if (m_JointType == EJointType::Slider)
		{
			// 未実装
		}
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