#pragma once
#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>
#include "../../Interface/IPhysicsEngine.h"
#include <btBulletDynamicsCommon.h>

#ifdef BT_ENABLE_THREADING
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include "CBulletTaskSchedulerManager.h"
#endif // BT_ENABLE_THREADING

namespace physics
{
	class CBulletPhysicsEngine : public IPhysicsEngine
	{
		bool m_Enabled;

#ifdef BT_ENABLE_THREADING
		std::unique_ptr<btCollisionConfiguration> m_CollisionConfigration;
		std::unique_ptr<btCollisionDispatcherMt> m_Dispathcer;
		std::unique_ptr<btBroadphaseInterface> m_OverlappingPairCache;
		std::unique_ptr<btSequentialImpulseConstraintSolverMt> m_Solver;
		std::unique_ptr<btDiscreteDynamicsWorldMt> m_DynamicsWorld;

		CBulletTaskSchedulerManager m_TaskSchedulerMgr;
#else
		std::unique_ptr<btCollisionConfiguration> m_CollisionConfigration;
		std::unique_ptr<btCollisionDispatcher> m_Dispathcer;
		std::unique_ptr<btBroadphaseInterface> m_OverlappingPairCache;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_Solver;
		std::unique_ptr<btDiscreteDynamicsWorld> m_DynamicsWorld;
#endif // BT_ENABLE_THREADING

	public:
		CBulletPhysicsEngine();
		virtual ~CBulletPhysicsEngine();

		virtual bool IsEnabled() const override;
		virtual void SetEnabled(bool Flag) override;

		virtual bool Initialize() override;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool Kinematic, const SRigidbodyParam& RBParam) override;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(float Radius, bool Kinematic, const SRigidbodyParam& RBParam) override;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsCapsule(float Radius, float Height, bool Kinematic, const SRigidbodyParam& RBParam) override;

		virtual bool Update(float DeltaTime) override;

#ifdef BT_ENABLE_THREADING
		btDiscreteDynamicsWorldMt* GetDynamicsWorld();
#else
		btDiscreteDynamicsWorld* GetDynamicsWorld();
#endif // BT_ENABLE_THREADING
		
	};
}
#endif // USE_PHYSICS