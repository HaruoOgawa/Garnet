#ifdef USE_PHYSICS
#include "CBulletPhysicsEngine.h"
#include "../../Message/Console.h"
#include "CBulletBox.h"
#include "CBulletSphere.h"
#include "CBulletCapsule.h"

namespace physics
{
	CBulletPhysicsEngine::CBulletPhysicsEngine():
		m_PhysicsTime(0.0f),
		m_CollisionConfigration(nullptr),
		m_Dispathcer(nullptr),
		m_OverlappingPairCache(nullptr),
		m_Solver(nullptr),
		m_DynamicsWorld(nullptr)
	{
	}

	CBulletPhysicsEngine::~CBulletPhysicsEngine()
	{
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld.reset();
			m_DynamicsWorld = nullptr;
		}

		if (m_Solver)
		{
			m_Solver.reset();
			m_Solver = nullptr;
		}

		if (m_OverlappingPairCache)
		{
			m_OverlappingPairCache.reset();
			m_OverlappingPairCache = nullptr;
		}

		if (m_Dispathcer)
		{
			m_Dispathcer.reset();
			m_Dispathcer = nullptr;
		}

		if (m_CollisionConfigration)
		{
			m_CollisionConfigration.reset();
			m_CollisionConfigration = nullptr;
		}
	}

	bool CBulletPhysicsEngine::Initialize()
	{
		// 物理エンジンの設定オブジェクトを初期化
		m_CollisionConfigration = std::make_unique<btDefaultCollisionConfiguration>();

		// dispatherを初期化. 物理計算を実行するオブジェクトのこと
		m_Dispathcer = std::make_unique<btCollisionDispatcher>(m_CollisionConfigration.get());

		// 物理演算のキャッシュ用？
		m_OverlappingPairCache = std::make_unique<btDbvtBroadphase>();

		// Constraint Solver. つまり衝突によるめり込み解決する責務を担っているオブジェクト
		m_Solver = std::make_unique<btSequentialImpulseConstraintSolver>();

		// dynamics world. 物理演算を行う仮想世界
		m_DynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(m_Dispathcer.get(), m_OverlappingPairCache.get(), m_Solver.get(), m_CollisionConfigration.get());

		// 重力を設定
		m_DynamicsWorld->setGravity(btVector3(0.0f, -9.8f * 10.0f, 0.0f));

		return true;
	}

	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletBox> Box = std::make_shared<CBulletBox>(BoxHalfSize, Kinematic, Mass, RBParam);

		return Box;
	}
	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsSphere(float Radius, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletSphere> Sphere = std::make_shared<CBulletSphere>(Radius, Kinematic, Mass, RBParam);

		return Sphere;
	}

	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsCapsule(float Radius, float Height, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletCapsule> capsule = std::make_shared<CBulletCapsule>(Radius, Height, Kinematic, Mass, RBParam);

		return capsule;
	}

	bool CBulletPhysicsEngine::Update(float DeltaTime)
	{
		m_PhysicsTime += DeltaTime;

		if (m_PhysicsTime < (1.0f / 30.0f)) return true;

		m_PhysicsTime = 0.0f;

		if (m_DynamicsWorld)
		{
			// timeStepは定数の方が軽いのでひとまず定数にしておく
			m_DynamicsWorld->stepSimulation(1.0f / 30.0f, 10);
			//m_DynamicsWorld->stepSimulation(DeltaTime, 10);
		}

		return true;
	}

	btDiscreteDynamicsWorld* CBulletPhysicsEngine::GetDynamicsWorld()
	{
		return m_DynamicsWorld.get();
	}
}
#endif // USE_PHYSICS