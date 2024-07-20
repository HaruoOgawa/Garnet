#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "CKeyFrame.h"
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

	enum class EInterpolateValueType
	{
		NONE,
		QUATERNION,
		MODELMATRIX,
	};

	class CAnimationSampler
	{
		EInterpolationType m_InterpolationType;
		std::vector<std::shared_ptr<animation::CKeyFrame>> m_KeyFrameList;

		float m_StartTime;
		float m_EndTime;

		int m_SelfSamplerIndex;
	private:
		float GetInterpolateValue(float CurrentTime, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);

		std::vector<float> CopyFromNumComponent(int NumComponent, const std::vector<float>& Src, int Offset);
		
		bool DoStepInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoSphericalLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoModelMatrixLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoCubicSplineInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);
		bool DoModelMatrixSplineInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);

		glm::vec2 CalculateSplinePont(float t, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
	public:
		CAnimationSampler(EInterpolationType InterpolationType);
		virtual ~CAnimationSampler();

		EInterpolationType GetInterpolationType() const;

		bool CreateKeyFrame(math::EValueType Type, const std::vector<float>& inputList, const std::vector<float>& outputList);
		void AddKeyFrame(const std::shared_ptr<animation::CKeyFrame>& KeyFrame);
		const std::vector<std::shared_ptr<animation::CKeyFrame>>& GetKeyFrameList() const;
		std::vector<std::shared_ptr<animation::CKeyFrame>> GetKeyFrameListFromRange(float FirstTime, float SecondTime);
		void SetKeyFrameInput(const std::shared_ptr<animation::CKeyFrame>& KeyFrame, float NewInput);

		void CalcStartEndTime();

		// ToDo: SetStartTimeとSetEndTimeは削除して全てCalcStartEndTimeにする
		void SetStartTime(float StartTime);
		float GetStartTime() const;
		void SetEndTime(float EndTime);
		float GetEndTime() const;

		void SetSelfSamplerIndex(int Index);

		bool IsEnd(float CurrentTime);

		bool ComputeCurrentFrame(float CurrentTime, bool IsLoop, std::vector<float>& Value, EInterpolateValueType ValueType);

		bool GetNeedKeyFrame(float CurrentTime, std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, std::shared_ptr<animation::CKeyFrame>& NextKeyFrame);

		// ボーンに基づく現在のフレームを取得
		static std::shared_ptr<CKeyFrame> GetCurrentKeyFrameBasedBone(float CurrentTime, EHumanoidBones BoneName, const std::unordered_map<animation::EHumanoidBones, std::vector<std::shared_ptr<animation::CKeyFrame>>>& FrameMatrixMap);
	};
}