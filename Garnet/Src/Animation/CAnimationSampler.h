#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "CKeyFrame.h"
#include "EAnimationTarget.h"

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

		float m_StartTime;
		float m_EndTime;
	private:
		std::vector<float> CopyFromNumComponent(int NumComponent, const std::vector<float>& Src, int Offset);
		int GetNumComponentsInType(EKeyFrameType Type);

		bool GetNeedKeyFrame(float CurrentTime, std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);

		bool DoStepInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoSphericalLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoModelMatrixLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoCubicSplineInterpolation(float CurrentTime, std::vector<float>& Value, EAnimationTarget AnimationTarget, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
	public:
		CAnimationSampler(EInterpolationType InterpolationType);
		virtual ~CAnimationSampler();

		EInterpolationType GetInterpolationType() const;

		bool CreateKeyFrame(EKeyFrameType Type, const std::vector<float>& inputList, const std::vector<float>& outputList);
		void AddKeyFrame(const std::shared_ptr<animation::CKeyFrame>& KeyFrame);
		const std::vector<std::shared_ptr<animation::CKeyFrame>>& GetKeyFrameList() const;

		void SetStartTime(float StartTime);
		float GetStartTime() const;
		void SetEndTime(float EndTime);
		float GetEndTime() const;

		bool ComputeCurrentFrame(float CurrentTime, std::vector<float>& Value, EAnimationTarget AnimationTarget);
	};
}