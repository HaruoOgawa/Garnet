#include "CAnimationSampler.h"

namespace animation
{
	CAnimationSampler::CAnimationSampler(EInterpolationType InterpolationType):
		m_InterpolationType(InterpolationType),
		m_StartTime(0.0f),
		m_EndTime(0.0f)
	{
	}

	CAnimationSampler::~CAnimationSampler()
	{
	}

	bool CAnimationSampler::CreateKeyFrame(EKeyFrameType Type, const std::vector<float>& inputList, const std::vector<float>& outputList)
	{
		const int NumComponent = GetNumComponentsInType(Type);

		if (NumComponent == -1) return false;

		if (inputList.size() != outputList.size() / NumComponent) return false;

		for (int i = 0; i < inputList.size(); i++)
		{
			std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(Type);

			// input
			KeyFrame->SetInput(inputList[i]);

			// output
			KeyFrame->SetOutput(CopyFromNumComponent(NumComponent, outputList, (i * NumComponent) ));

			//
			m_KeyFrameList.push_back(KeyFrame);
		}

		if (m_KeyFrameList.size() > 0)
		{
			m_StartTime = m_KeyFrameList[0]->GetInput();
			m_EndTime = m_KeyFrameList[m_KeyFrameList.size() - 1]->GetInput();
		}
		 
		return true;
	}

	std::vector<float> CAnimationSampler::CopyFromNumComponent(int NumComponent, const std::vector<float>& Src, int Offset)
	{
		std::vector<float> Dst;

		Dst.resize(NumComponent);
		std::memcpy(&Dst[0], &Src[Offset], NumComponent * sizeof(float));

		return Dst;
	}

	int CAnimationSampler::GetNumComponentsInType(EKeyFrameType Type)
	{
		if (Type == EKeyFrameType::KEYFRAME_TYPE_SCALAR) 
		{
			return 1;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_VEC2) 
		{
			return 2;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_VEC3) 
		{
			return 3;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_VEC4) 
		{
			return 4;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_MAT2) 
		{
			return 4;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_MAT3) 
		{
			return 9;
		}
		else if (Type == EKeyFrameType::KEYFRAME_TYPE_MAT4) 
		{
			return 16;
		}
		else 
		{
			// Unknown component type
			return -1;
		}
	}

	bool CAnimationSampler::GetCurrentFrame(float CurrentTime, std::vector<float>& Value, bool IsRot)
	{
		float CalcCurrentTime = glm::mod(CurrentTime, m_EndTime);

		// 処理対処のキーフレームを取得
		std::shared_ptr<animation::CKeyFrame> PrevKeyFrame = nullptr;
		std::shared_ptr<animation::CKeyFrame> NextKeyFrame = nullptr;
		
		if (!GetNeedKeyFrame(CalcCurrentTime, PrevKeyFrame, NextKeyFrame)) return false;

		// 補完されたVakueを取得
		switch (m_InterpolationType)
		{
		case animation::EInterpolationType::STEP:
			if (!DoStepInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			break;
		case animation::EInterpolationType::LINEAR:
			if (IsRot)
			{
				if(!DoSlerpInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			}
			else
			{
				if (!DoLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			}
			break;
		case animation::EInterpolationType::CUBICSPLINE:
			if (!DoCubicSplineInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			break;
		default:
			break;
		}

		return true;
	}

	bool CAnimationSampler::GetNeedKeyFrame(float CurrentTime, std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// Next
		const auto& val = std::find_if(m_KeyFrameList.begin(), m_KeyFrameList.end(), [&](std::shared_ptr<CKeyFrame>& f) {  bool r = (CurrentTime < f->GetInput()); if (r) { NextKeyFrame = f; } return r; });
		if (val == m_KeyFrameList.end()) return false;

		// Prev
		size_t NextIndex = std::distance(m_KeyFrameList.begin(), val);

		// NextIndexは0よりも大きい必要がある
		if (NextIndex <= 0 || NextIndex >= m_KeyFrameList.size()) return false;

		PrevKeyFrame = m_KeyFrameList[NextIndex - 1];

		//
		if (PrevKeyFrame == nullptr || NextKeyFrame == nullptr) return false;

		return true;
	}

	bool CAnimationSampler::DoStepInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		return true;
	}

	bool CAnimationSampler::DoLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// 線形補完(Linear)
		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		float L = (CurrentTime - PrevTime) / (NextTime - PrevTime);

		const auto& PrevValue = PrevKeyFrame->GetOutput();
		const auto& NextValue = NextKeyFrame->GetOutput();

		if (PrevValue.size() != NextValue.size()) return false;

		for (int i = 0; i < PrevValue.size(); i++)
		{
			float val = (1.0f - L) * PrevValue[i] + L * NextValue[i];

			Value.push_back(val);
		}

		return true;
	}

	bool CAnimationSampler::DoSlerpInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// 回転の場合は 0.0・90・-180・-90・0と指定されることもあるので通常の補完ではなくSlerpを使用する必要がある
		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		float L = (CurrentTime - PrevTime) / (NextTime - PrevTime);

		const auto& PrevValue = PrevKeyFrame->GetOutput();
		const auto& NextValue = NextKeyFrame->GetOutput();

		if (PrevValue.size() != NextValue.size()) return false;
		if (PrevValue.size() != 4) return false;

		glm::quat PrevQuat = glm::quat(PrevValue[0], PrevValue[1], PrevValue[2], PrevValue[3]);
		glm::quat NextQuat = glm::quat(NextValue[0], NextValue[1], NextValue[2], NextValue[3]);

		glm::quat dstQuat = glm::slerp(PrevQuat, NextQuat, L);

		Value.push_back(dstQuat.x);
		Value.push_back(dstQuat.y);
		Value.push_back(dstQuat.z);
		Value.push_back(dstQuat.w);

		return true;
	}

	bool CAnimationSampler::DoCubicSplineInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		return true;
	}
}