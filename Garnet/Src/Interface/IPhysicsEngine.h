#pragma once
#ifdef USE_PHYSICS
namespace physics
{
	class IPhysicsEngine
	{
	public:
		virtual bool Initialize() = 0;
	};
}
#endif // USE_PHYSICS