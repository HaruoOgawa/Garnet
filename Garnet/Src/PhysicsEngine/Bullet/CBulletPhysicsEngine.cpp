#ifdef USE_PHYSICS
#include "CBulletPhysicsEngine.h"
#include "../../Message/Console.h"
#include "CBulletBox.h"
#include "CBulletSphere.h"
#include "CBulletCapsule.h"

namespace physics
{
	CBulletPhysicsEngine::CBulletPhysicsEngine():
		m_Enabled(true),
		m_CollisionConfigration(nullptr),
		m_Dispathcer(nullptr),
		m_OverlappingPairCache(nullptr),
		m_Solver(nullptr),
		m_DynamicsWorld(nullptr)
	{
	}

	CBulletPhysicsEngine::~CBulletPhysicsEngine()
	{
#ifdef BT_ENABLE_THREADING
		m_TaskSchedulerMgr.Shutdown();
#endif // BT_ENABLE_THREADING


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

	bool CBulletPhysicsEngine::IsEnabled() const
	{
		return m_Enabled;
	}

	void CBulletPhysicsEngine::SetEnabled(bool Flag)
	{
		m_Enabled = Flag;
	}

	bool CBulletPhysicsEngine::Initialize()
	{
#ifdef BT_ENABLE_THREADING
		// マルチスレッドモード //////////////////////////////////////////////////////
		// 有効にするには以下のプリプロセッサを使用する
		// BT_THREADSAFE
		// BT_USE_OPENMP
		// BT_USE_PPL
		// BT_ENABLE_THREADING
		///////////////////////////////////////////////////////////////////////////
		m_TaskSchedulerMgr.Init();

		btDefaultCollisionConstructionInfo cci;
		cci.m_defaultMaxCollisionAlgorithmPoolSize = 80000;
		cci.m_defaultMaxPersistentManifoldPoolSize = 80000;

		m_CollisionConfigration = std::make_unique<btDefaultCollisionConfiguration>(cci);

		btCollisionDispatcherMt* multi_dispacher = new btCollisionDispatcherMt(m_CollisionConfigration.get(), 40);

		m_OverlappingPairCache = std::make_unique<btDbvtBroadphase>();

		btConstraintSolverPoolMt* solverPool;
		{
			btConstraintSolver* solvers[BT_MAX_THREAD_COUNT];
			int maxThreadCount = BT_MAX_THREAD_COUNT;
			for (int i = 0; i < maxThreadCount; ++i)
			{
				solvers[i] = new btSequentialImpulseConstraintSolverMt();
			}
			solverPool = new btConstraintSolverPoolMt(solvers, maxThreadCount);
		}

		btSequentialImpulseConstraintSolverMt* solverMt = new btSequentialImpulseConstraintSolverMt();

		// dynamics world. 物理演算を行う仮想世界
		m_DynamicsWorld = std::make_unique<btDiscreteDynamicsWorldMt>(multi_dispacher, m_OverlappingPairCache.get(), solverPool, solverMt, m_CollisionConfigration.get());

		// 重力を設定
		m_DynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
#else
		// シングルスレッドモード
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
		m_DynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
#endif // BT_ENABLE_THREADING

		return true;
	}

	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool Kinematic, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletBox> Box = std::make_shared<CBulletBox>(BoxHalfSize, Kinematic, RBParam);

		return Box;
	}
	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsSphere(float Radius, bool Kinematic, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletSphere> Sphere = std::make_shared<CBulletSphere>(Radius, Kinematic, RBParam);

		return Sphere;
	}

	std::shared_ptr<IPhysicsObject> CBulletPhysicsEngine::CreatePhysicsCapsule(float Radius, float Height, bool Kinematic, const SRigidbodyParam& RBParam)
	{
		std::shared_ptr<CBulletCapsule> capsule = std::make_shared<CBulletCapsule>(Radius, Height, Kinematic, RBParam);

		return capsule;
	}

	bool CBulletPhysicsEngine::Update(float DeltaTime)
	{
		if (!m_Enabled) return true;

		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->stepSimulation(DeltaTime, 10, 1.0 / 60.0f);
		}

		return true;
	}

#ifdef BT_ENABLE_THREADING
	btDiscreteDynamicsWorldMt* CBulletPhysicsEngine::GetDynamicsWorld()
#else
	btDiscreteDynamicsWorld* CBulletPhysicsEngine::GetDynamicsWorld()
#endif // BT_ENABLE_THREADING
	{
		return m_DynamicsWorld.get();
	}
}
#endif // USE_PHYSICS