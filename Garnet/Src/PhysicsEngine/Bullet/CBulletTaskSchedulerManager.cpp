#if defined(USE_PHYSICS) && defined(BT_ENABLE_THREADING)
#include "CBulletTaskSchedulerManager.h"

namespace physics
{
	CBulletTaskSchedulerManager::CBulletTaskSchedulerManager()
	{
	}

	void CBulletTaskSchedulerManager::Init()
	{
		AddTaskScheduler(btGetSequentialTaskScheduler());
#if BT_THREADSAFE
		if (btITaskScheduler* ts = btCreateDefaultTaskScheduler())
		{
			m_allocatedTaskSchedulers.push_back(ts);
			AddTaskScheduler(ts);
		}
		AddTaskScheduler(btGetOpenMPTaskScheduler());
		AddTaskScheduler(btGetTBBTaskScheduler());
		AddTaskScheduler(btGetPPLTaskScheduler());
		if (GetNumTaskSchedulers() > 1)
		{
			// prefer a non-sequential scheduler if available
			btSetTaskScheduler(m_taskSchedulers[1]);
		}
		else
		{
			btSetTaskScheduler(m_taskSchedulers[0]);
		}
#endif  // #if BT_THREADSAFE
	}

	void CBulletTaskSchedulerManager::Shutdown()
	{
		for (int i = 0; i < m_allocatedTaskSchedulers.size(); ++i)
		{
			delete m_allocatedTaskSchedulers[i];
		}
		m_allocatedTaskSchedulers.clear();
	}

	void CBulletTaskSchedulerManager::AddTaskScheduler(btITaskScheduler* ts)
	{
		if (ts)
		{
#if BT_THREADSAFE
			// if initial number of threads is 0 or 1,
			if (ts->getNumThreads() <= 1)
			{
				// for OpenMP, TBB, PPL set num threads to number of logical cores
				ts->setNumThreads(ts->getMaxNumThreads());
			}
#endif  // #if BT_THREADSAFE
			m_taskSchedulers.push_back(ts);
		}
	}

	int CBulletTaskSchedulerManager::GetNumTaskSchedulers() const 
	{ 
		return m_taskSchedulers.size(); 
	}

	btITaskScheduler* CBulletTaskSchedulerManager::GetTaskScheduler(int i) 
	{ 
		return m_taskSchedulers[i]; 
	}
}
#endif // USE_PHYSICS && BT_ENABLE_THREADING