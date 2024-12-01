#pragma once

#include <memory>
#include "EJointType.h"
#include "SJointParam.h"

namespace physics
{
	class IPhysicsObject;

	struct SConstraintData
	{
		std::shared_ptr<IPhysicsObject> FixedObject = nullptr;
		EJointType JointType = EJointType::NONE;
		SJointParam JParam = {};

		SConstraintData(const std::shared_ptr<IPhysicsObject>& object, EJointType t, const SJointParam& param) :
			FixedObject(object),
			JointType(t),
			JParam(param)
		{
		}
	};
}