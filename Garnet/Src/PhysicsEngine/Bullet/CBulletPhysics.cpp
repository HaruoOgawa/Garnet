#ifdef USE_PHYSICS
#include "CBulletPhysics.h"
#include "../../Debug/Message/Console.h"
#include "CBulletBox.h"
#include "CBulletSphere.h"

namespace physics
{
	CBulletPhysics::CBulletPhysics():
		m_CollisionConfigration(nullptr),
		m_Dispathcer(nullptr),
		m_OverlappingPairCache(nullptr),
		m_Solver(nullptr),
		m_DynamicsWorld(nullptr)
	{
	}

	CBulletPhysics::~CBulletPhysics()
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

	bool CBulletPhysics::Initialize()
	{
		//if (!HelloWorldTestCode()) return false;

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
		m_DynamicsWorld->setGravity(btVector3(0, -10, 0));

		return true;
	}

	std::shared_ptr<IPhysicsObject> CBulletPhysics::CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool IsStatic, float Mass)
	{
		std::shared_ptr<CBulletBox> Box = std::make_shared<CBulletBox>(BoxHalfSize, IsStatic, Mass);

		return Box;
	}
	std::shared_ptr<IPhysicsObject> CBulletPhysics::CreatePhysicsSphere(float Radius, bool IsStatic, float Mass)
	{
		std::shared_ptr<CBulletSphere> Sphere = std::make_shared<CBulletSphere>(Radius, IsStatic, Mass);

		return Sphere;
	}

	bool CBulletPhysics::Update(float DeltaTime)
	{
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->stepSimulation(DeltaTime, 10);
		}

		return true;
	}

	btDiscreteDynamicsWorld* CBulletPhysics::GetDynamicsWorld()
	{
		return m_DynamicsWorld.get();
	}

	bool CBulletPhysics::HelloWorldTestCode()
	{
		Console::Log("[CPP] Bullet Init Start\n");

		// 物理エンジンの設定オブジェクトを初期化
		btDefaultCollisionConfiguration* collisionConfigration = new btDefaultCollisionConfiguration();

		// dispatherを初期化. 物理計算を実行するオブジェクトのこと
		btCollisionDispatcher* dispathcer = new btCollisionDispatcher(collisionConfigration);

		// 物理演算のキャッシュ用？
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		// Constraint Solver. つまり衝突によるめり込み解決する責務を担っているオブジェクト
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

		// dynamics world. 物理演算を行う仮想世界
		btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispathcer, overlappingPairCache, solver, collisionConfigration);

		// 重力を設定
		dynamicsWorld->setGravity(btVector3(0, -10, 0));

		// Collision Shapes
		btAlignedObjectArray<btCollisionShape*> collisionShapes;

		// 地面のキューブ 固定されている
		{
			// CollisionShapeはコライダー. 衝突判定に使用
			btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.0), btScalar(50.0), btScalar(50.0)));

			collisionShapes.push_back(groundShape);

			// Transform
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, -56, 0));

			// 質量
			btScalar mass(0.0);

			// Bulletは質量が0のものはStatic(固定されている)、そうでないものはDynamic(物理演算で動く)として扱われる
			bool IsDynamic = (mass != 0.0f);

			// Inertiaは慣性の意味
			btVector3 localInertia(0, 0, 0);
			// 慣性力の計算でここではそれを0に初期化している
			if (IsDynamic)
			{
				groundShape->calculateLocalInertia(mass, localInertia);
			}

			// MotoinState. 補間だったり他のアクティブオブジェクトとの同期に使用される
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			
			// RigidBodyの設定. 物理演算に使用するオブジェクト.物理演算に関するパラメーターを持っている
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
			
			// RigidBodyを物理演算ワールドに追加
			dynamicsWorld->addRigidBody(body);
		}

		// 物理演算で動くスフィア
		{
			btCollisionShape* colShape = new btSphereShape(btScalar(1.0));
			collisionShapes.push_back(colShape);

			btScalar mass(1.0f);

			bool IsDynamic = (mass != 0.0f);

			btVector3 localIntertia(0, 0, 0);
			if (IsDynamic)
			{
				colShape->calculateLocalInertia(mass, localIntertia);
			}

			btTransform startTransform;
			startTransform.setIdentity();

			startTransform.setOrigin(btVector3(2, 10, 0));

			btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localIntertia);
			btRigidBody* body = new btRigidBody(rbInfo);

			dynamicsWorld->addRigidBody(body);
		}

		//
		for (int i = 0; i < 150; i++)
		{
			// 60FPSでシミュレーションを進める
			dynamicsWorld->stepSimulation(1.0f / 60.0f, 10);

			Console::Log("[CPP] dynamicsWorld->stepSimulation ____________________________\n");

			// オブジェクトの位置をプリントする
			for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
			{
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
				
				// btCollisionObjectをbtRigidBodyにアッパーキャスト
				btRigidBody* body = btRigidBody::upcast(obj);

				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = obj->getWorldTransform();
				}

				Console::Log("[CPP] Physic World Pos => x: %f, y: %f, z: %f\n", static_cast<float>(trans.getOrigin().getX()), static_cast<float>(trans.getOrigin().getY()), static_cast<float>(trans.getOrigin().getZ()));
			}
		}

		// メモリ解放
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);

			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}

			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		for(int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;

			delete shape;
		}
		
		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispathcer;
		delete collisionConfigration;

		collisionShapes.clear();

		Console::Log("[CPP] Bullet Init End\n");

		return true;
	}
}
#endif // USE_PHYSICS