#pragma once
#ifdef USE_ANIMATION

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "CKeyFrame.h"
#include "EAnimationTarget.h"
#include "EHumanoidBones.h"

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
		std::vector<float> GetDefaultValueFromAnimationTarget(EAnimationTarget AnimationTarget);

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

		bool IsEnd(float CurrentTime);

		bool ComputeCurrentFrame(float CurrentTime, bool IsLoop, std::vector<float>& Value, EAnimationTarget AnimationTarget);

		// ボーンに基づく現在のフレームを取得
		static std::shared_ptr<CKeyFrame> GetCurrentKeyFrameBasedBone(float CurrentTime, EHumanoidBones BoneName, const std::unordered_map<animation::EHumanoidBones, std::vector<std::shared_ptr<animation::CKeyFrame>>>& FrameMatrixMap);
	};
}

#endif // USE_ANIMATION