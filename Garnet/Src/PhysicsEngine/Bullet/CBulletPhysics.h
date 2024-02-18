#pragma once
#ifdef USE_PHYSICS
#include <memory>
#include <glm/glm.hpp>
#include "../../Interface/IPhysicsEngine.h"
#include <btBulletDynamicsCommon.h>

namespace physics
{
	class CBulletPhysics : public IPhysicsEngine
	{
		std::unique_ptr<btDefaultCollisionConfiguration> m_CollisionConfigration;
		std::unique_ptr<btCollisionDispatcher> m_Dispathcer;
		std::unique_ptr<btBroadphaseInterface> m_OverlappingPairCache;
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_Solver;
		std::unique_ptr<btDiscreteDynamicsWorld> m_DynamicsWorld;
	private:
		bool HelloWorldTestCode();
	public:
		CBulletPhysics();
		virtual ~CBulletPhysics();

		virtual bool Initialize() override;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool IsStatic, float Mass) override;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(float Radius, bool IsStatic, float Mass) override;

		virtual bool Update(float DeltaTime) override;

		btDiscreteDynamicsWorld* GetDynamicsWorld();
	};
}
#endif // USE_PHYSICS