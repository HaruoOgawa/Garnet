#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "CKeyFrame.h"

namespace animation
{
	enum class EInterpolationType
	{
		NONE = -1,

		STEP,
		LINEAR,
		CUBICSPLINE,
	};

	class CAnimationSampler
	{
		EInterpolationType m_InterpolationType;
		std::vector<std::shared_ptr<animation::CKeyFrame>> m_KeyFrameList;
	private:
		std::vector<float> CopyFromNumComponent(int NumComponent, const std::vector<float>& Src, int Offset);
		int GetNumComponentsInType(EKeyFrameType Type);
	public:
		CAnimationSampler(EInterpolationType InterpolationType);
		virtual ~CAnimationSampler();

		bool CreateKeyFrame(EKeyFrameType Type, const std::vector<float>& inputList, const std::vector<float>& outputList);
	};
}