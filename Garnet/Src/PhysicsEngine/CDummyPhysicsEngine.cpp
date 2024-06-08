#ifndef USE_PHYSICS
#include "CDummyPhysicsEngine.h"

namespace physics
{
	CDummyPhysicsEngine::CDummyPhysicsEngine()
	{
	}

	CDummyPhysicsEngine::~CDummyPhysicsEngine()
	{
	}

	bool CDummyPhysicsEngine::Initialize()
	{
		return true;
	}

	std::shared_ptr<IPhysicsObject> CDummyPhysicsEngine::CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		return nullptr;
	}

	std::shared_ptr<IPhysicsObject> CDummyPhysicsEngine::CreatePhysicsSphere(float Radius, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		return nullptr;
	}

	std::shared_ptr<IPhysicsObject> CDummyPhysicsEngine::CreatePhysicsCapsule(float Radius, float Height, bool Kinematic, float Mass, const SRigidbodyParam& RBParam)
	{
		return nullptr;
	}

	bool CDummyPhysicsEngine::Update(float DeltaTime)
	{
		return true;
	}
}
#endif // !USE_PHYSICS