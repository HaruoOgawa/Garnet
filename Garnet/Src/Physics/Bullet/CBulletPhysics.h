#pragma once
#ifdef USE_PHYSICS
#include "../../Interface/IPhysicsEngine.h"
#include <btBulletDynamicsCommon.h>

namespace physics
{
	class CBulletPhysics : public IPhysicsEngine
	{
	private:
		bool HelloWorldTestCode();
	public:
		CBulletPhysics();
		virtual ~CBulletPhysics();

		virtual bool Initialize() override;
	};
}
#endif // USE_PHYSICS