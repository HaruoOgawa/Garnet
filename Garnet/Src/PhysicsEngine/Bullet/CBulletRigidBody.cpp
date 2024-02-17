#ifdef USE_PHYSICS
#include "CBulletRigidBody.h"

namespace physics
{
	CBulletRigidBody::CBulletRigidBody(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& Origin, bool IsStatic, float Mass):
		m_pDynamicWorld(pDynamicWorld),
		m_MotionState(nullptr),
		m_Rigidbody(nullptr)
	{
		Create(pDynamicWorld, pCollisionShape, Origin, IsStatic, Mass);
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

	bool CBulletRigidBody::Create(btDiscreteDynamicsWorld* pDynamicWorld, btCollisionShape* pCollisionShape, const glm::vec3& Origin, bool IsStatic, float Mass)
	{
		// Transform
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(Origin.x, Origin.y, Origin.z));

		// 質量
		btScalar bodyMass(Mass);

		// Bulletは質量が0のものはStatic(固定されている)、そうでないものはDynamic(物理演算で動く)として扱われる
		bool IsDynamic = (!IsStatic && bodyMass != 0.0f);

		// Inertiaは慣性の意味
		btVector3 localInertia(0, 0, 0);
		// 慣性力の計算でここではそれを0に初期化している
		if (IsDynamic)
		{
			pCollisionShape->calculateLocalInertia(bodyMass, localInertia);
		}

		// MotoinState. 補間だったり他のアクティブオブジェクトとの同期に使用される
		m_MotionState = std::make_shared<btDefaultMotionState>(transform);

		// RigidBodyの設定. 物理演算に使用するオブジェクト.物理演算に関するパラメーターを持っている
		btRigidBody::btRigidBodyConstructionInfo rbInfo(bodyMass, m_MotionState.get(), pCollisionShape, localInertia);
		m_Rigidbody = std::make_shared<btRigidBody>(rbInfo);

		// RigidBodyを物理演算ワールドに追加
		pDynamicWorld->addRigidBody(m_Rigidbody.get());

		return true;
	}
}
#endif