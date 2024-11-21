#pragma once

#ifdef BT_ENABLE_THREADING
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>

namespace physics
{
	class CBulletTaskSchedulerManager
	{
		btAlignedObjectArray<btITaskScheduler*> m_taskSchedulers;
		btAlignedObjectArray<btITaskScheduler*> m_allocatedTaskSchedulers;

	public:
		CBulletTaskSchedulerManager();

		void Init();
		
		void Shutdown();

		void AddTaskScheduler(btITaskScheduler* ts);
		
		int GetNumTaskSchedulers() const;

		btITaskScheduler* GetTaskScheduler(int i);
	};
}
#endif // BT_ENABLE_THREADING