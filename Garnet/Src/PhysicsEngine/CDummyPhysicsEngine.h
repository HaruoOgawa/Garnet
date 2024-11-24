#pragma once

#ifndef USE_PHYSICS
#include "../../Interface/IPhysicsEngine.h"

// USE_PHYSICSがない時に使うダミーの空物理エンジン
// 物理エンジンをバイナリに含めるかどうか引数を変えるのはめんどくさいのでこれを使う
// GUIEngineや他の部分にもDummyの考え方は流用する
namespace physics
{
	class CDummyPhysicsEngine : public IPhysicsEngine
	{
	public:
		CDummyPhysicsEngine();
		virtual ~CDummyPhysicsEngine();

		virtual bool IsEnabled() const override;
		virtual void SetEnabled(bool Flag) override;

		virtual bool Initialize() override;

		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsBox(const glm::vec3& BoxHalfSize, bool Kinematic, float Mass, const SRigidbodyParam& RBParam) override;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsSphere(float Radius, bool Kinematic, float Mass, const SRigidbodyParam& RBParam) override;
		virtual std::shared_ptr<IPhysicsObject> CreatePhysicsCapsule(float Radius, float Height, bool Kinematic, float Mass, const SRigidbodyParam& RBParam) override;

		virtual bool Update(float DeltaTime) override;
	};
}
#endif // !USE_PHYSICS