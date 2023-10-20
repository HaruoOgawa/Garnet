#include "CAnimationSampler.h"

namespace animation
{
	CAnimationSampler::CAnimationSampler(EInterpolationType InterpolationType):
		m_InterpolationType(InterpolationType)
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
}