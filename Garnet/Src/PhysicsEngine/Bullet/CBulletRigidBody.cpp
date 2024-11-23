#ifdef USE_PHYSICS
#include "CBulletRigidBody.h"
#include "../../Message/Console.h"

namespace physics
{
	CBulletRigidBody::CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsKinematic, float Mass, const SRigidbodyParam& RBParam):
		m_RBParam(RBParam),
		m_pDynamicWorld(pDynamicWorld),
		m_MotionState(nullptr),
		m_Rigidbody(nullptr),
		m_JointType(EJointType::NONE)
	{
		Create(pDynamicWorld, pCollisionShape, WorldPos, WorldRotate, IsKinematic, Mass, RBParam);
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

	const SRigidbodyParam& CBulletRigidBody::GetRbParam() const
	{
		return m_RBParam;
	}

	bool CBulletRigidBody::Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& WorldPos, const glm::quat& WorldRotate, bool IsKinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		// Transform
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(WorldPos.x, WorldPos.y, WorldPos.z));
		transform.setRotation(btQuaternion(WorldRotate.x, WorldRotate.y, WorldRotate.z, WorldRotate.w));

		// 質量
		btScalar bodyMass(0.0f);

		if (RBParam.PhysicsType != EPhysicsType::STATIC)
		{
			bodyMass = Mass;
		}

		// Inertiaは慣性の意味
		btVector3 localInertia(0, 0, 0);
		
		btDefaultMotionState* pMotionState = nullptr;

		// Bulletは質量が0のものはStatic(固定されている)、そうでないものはDynamic(物理演算で動く)として扱われる
		// Kinematicは物理演算上は固定されているが、ユーザーが動かすのでMotionStateを作る
		if (bodyMass != 0.0f || IsKinematic)
		{
			// 慣性力の計算でここではそれを0に初期化している
			pCollisionShape->calculateLocalInertia(bodyMass, localInertia);

			// MotoinState. 補間だったり他のアクティブオブジェクトとの同期に使用される
			// staticオブジェクトだとMotioStateのTransformは更新されないのでnullptrにしておく必要がある(代わりにRigidBodyの方が更新される)
			// https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12634
			m_MotionState = std::make_shared<btDefaultMotionState>(transform);

			pMotionState = m_MotionState.get();
		}

		// RigidBodyの設定. 物理演算に使用するオブジェクト.物理演算に関するパラメーターを持っている
		btRigidBody::btRigidBodyConstructionInfo rbInfo(bodyMass, pMotionState, pCollisionShape, localInertia);
		m_Rigidbody = std::make_shared<btRigidBody>(rbInfo);

		m_Rigidbody->setDamping(RBParam.TransDamping, RBParam.RotateDamping);
		m_Rigidbody->setRestitution(RBParam.Repulsion); // 反発係数の設定
		m_Rigidbody->setFriction(RBParam.Friction); // 摩擦係数の設定

		m_Rigidbody->setSleepingThresholds(0.01f, 0.01f); // 最適化用。物理演算を行わなくなるまでの閾値

		if (IsKinematic && RBParam.PhysicsType == EPhysicsType::STATIC)
		{
			// KinematicObjectとは動かすことのできるStaticObject
			// 言い換えるとユーザーが動かすことができる。動的オブジェクトを押したりすることはできるが、オブジェクトからは影響を受けない。つまり一方通行
			// http://bulletjpn.web.fc2.com/07_RigidBodyDynamics.html
			m_Rigidbody->setCollisionFlags(m_Rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

			m_Rigidbody->setActivationState(DISABLE_DEACTIVATION);
		}

		// ワールド座標をセットする
		//SetWorldTransform(transform);

		// RigidBodyを物理演算ワールドに追加
		// 非衝突グループの設定のビットマスクもここで設定する
		if (RBParam.UseCollideMask)
		{
			pDynamicWorld->addRigidBody(m_Rigidbody.get(), RBParam.group, ~RBParam.NoneCollideGroupFlag);
		}
		else
		{
			pDynamicWorld->addRigidBody(m_Rigidbody.get());
		}

		return true;
	}

	void CBulletRigidBody::Add6DofSpringConstraint(btDiscreteDynamicsWorld* pDynamicWorld, const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam)
	{
		m_JointType = EJointType::SPRING_6DOF;

		// Constraintsを追加
		// btGeneric6DofSpring2Constraint(*d6body0,*fixedBody1,frameInA,frameInB);
		// frameInAとframeInBはバネに例えるとバネの端点・剛体との接合点を表す. 二つの剛体にバネを挟むことをイメージするとわかりやすい. それは必ず２つの接合点があるはずである
		// frameInAはd6body0の接合点、frameInBのfixedBody1の接合点
		// frameInA・frameInBはバネの中心からのローカルオフセット(座標・回転)である
		// これらのローカル座標は中心のワールド座標に接合点のワールド座標の逆行列をかけることで求まる

		btTransform JointWorldTransform;
		{
			btMatrix3x3 rotMat;
			rotMat.setEulerZYX(JParam.JointRotate.x, JParam.JointRotate.y, JParam.JointRotate.z);

			JointWorldTransform.setIdentity();
			JointWorldTransform.setOrigin(btVector3(JParam.JointPos.x, JParam.JointPos.y, JParam.JointPos.z));
			JointWorldTransform.setBasis(rotMat);
		}

		btTransform localA;
		{
			localA = GetCurrentWorldTransform().inverse() * JointWorldTransform;
		}

		btTransform localB;
		{
			localB = FixedRigidbody->GetCurrentWorldTransform().inverse() * JointWorldTransform;
		}

		std::shared_ptr<btGeneric6DofSpringConstraint> Constraint = std::make_shared<btGeneric6DofSpringConstraint>(
			*m_Rigidbody.get(), 
			*FixedRigidbody->GetbtRigidBody().get(),
			localA,
			localB,
			true
		);

		// 関数名の通り移動できる範囲・回転できる範囲を設定
		{
			Constraint->setLinearLowerLimit(btVector3(JParam.LowerTransLimit.x, JParam.LowerTransLimit.y, JParam.LowerTransLimit.z));
			Constraint->setLinearUpperLimit(btVector3(JParam.UpperTransLimit.x, JParam.UpperTransLimit.y, JParam.UpperTransLimit.z));
			Constraint->setAngularLowerLimit(btVector3(JParam.LowerRotateLimit.x, JParam.LowerRotateLimit.y, JParam.LowerRotateLimit.z));
			Constraint->setAngularUpperLimit(btVector3(JParam.UpperRotateLimit.x, JParam.UpperRotateLimit.y, JParam.UpperRotateLimit.z));
		}

		// パラメーターを設定
		for(int a = 0; a < 3; a++)
		{
			if (JParam.TransSpring[a] != 0.0f)
			{
				Constraint->enableSpring(a, true);
				Constraint->setStiffness(a, JParam.TransSpring[a]); // Stiffness: 硬さ
			}
		}

		for (int b = 0; b < 3; b++)
		{
			if (JParam.RotateSpring[b] != 0.0f)
			{
				Constraint->enableSpring(b + 3, true);
				Constraint->setStiffness(b + 3, JParam.RotateSpring[b]); // Stiffness: 硬さ
			}
		}

		// 現在の位置をバネの釣り合いの位置(自然長)にする
		Constraint->setEquilibriumPoint();

		// 物理ワールドに追加
		pDynamicWorld->addConstraint(Constraint.get(), false);

		//
		m_6DofSpringConstraintList.push_back(Constraint);
	}

	void CBulletRigidBody::AlignConstraint(const std::shared_ptr<CBulletRigidBody>& FixedRigidbody, SJointParam JParam, const btTransform& JointWorldTransform)
	{
		if (m_JointType == EJointType::SPRING_6DOF)
		{
			for (const auto& Constraint : m_6DofSpringConstraintList)
			{
				btTransform localA;
				{
					localA = GetCurrentWorldTransform().inverse() * JointWorldTransform;
				}

				btTransform localB;
				{
					localB = FixedRigidbody->GetCurrentWorldTransform().inverse() * JointWorldTransform;
				}

				Constraint->setFrames(localA, localB);
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