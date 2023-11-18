#include "CAnimationSampler.h"
#include "../Math/CTransform.h"

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

	void CAnimationSampler::AddKeyFrame(const std::shared_ptr<animation::CKeyFrame>& KeyFrame)
	{
		m_KeyFrameList.push_back(KeyFrame);
	}

	const std::vector<std::shared_ptr<animation::CKeyFrame>>& CAnimationSampler::GetKeyFrameList() const
	{
		return m_KeyFrameList;
	}

	void CAnimationSampler::SetStartTime(float StartTime)
	{
		m_StartTime = StartTime;
	}

	void CAnimationSampler::SetEndTime(float EndTime)
	{
		m_EndTime = EndTime;
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

	bool CAnimationSampler::GetCurrentFrame(float CurrentTime, std::vector<float>& Value, EAnimationTarget AnimationTarget)
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
			{
				switch (AnimationTarget)
				{
				case animation::EAnimationTarget::ROTATION:
					// 回転のLinearの場合、Slerp( Spherical Linear Interpolation)を使用する必要がある
					// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-c-interpolation
					if (!DoSphericalLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				case animation::EAnimationTarget::TRANSLATION:
				case animation::EAnimationTarget::SCALE:
				case animation::EAnimationTarget::WEIGHTS:
					if (!DoLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				case animation::EAnimationTarget::MODELMATRIX:
					if (!DoModelMatrixLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				default:
					break;
				}
			}
			break;
		case animation::EInterpolationType::CUBICSPLINE:
			if (!DoCubicSplineInterpolation(CalcCurrentTime, Value, AnimationTarget, PrevKeyFrame, NextKeyFrame)) return false;
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

		// CurrentTimeがKeyFrameの最初よりも小さい時はPrevとNextにそれぞれ0と1のKeyFrameを割り当てる
		if (NextIndex <= 0 || NextIndex >= m_KeyFrameList.size())
		{
			NextKeyFrame = m_KeyFrameList[1];
			PrevKeyFrame = m_KeyFrameList[0];
		}
		else
		{
			PrevKeyFrame = m_KeyFrameList[NextIndex - 1];
		}
		
		if (PrevKeyFrame == nullptr || NextKeyFrame == nullptr) return false;

		return true;
	}

	bool CAnimationSampler::DoStepInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// Step: 補完機能はなく、常にPrevTimeのValueを参照するアニメーション
		const auto& PrevValue = PrevKeyFrame->GetOutput();

		for (int i = 0; i < PrevValue.size(); i++)
		{
			float val = PrevValue[i];

			Value.push_back(val);
		}

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

	bool CAnimationSampler::DoSphericalLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// 回転の場合は 0.0・90・-180・-90・0と指定されることもあるので通常の補完ではなくSlerpを使用する必要がある
		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		float L = (CurrentTime - PrevTime) / (NextTime - PrevTime);

		const auto& PrevValue = PrevKeyFrame->GetOutput();
		const auto& NextValue = NextKeyFrame->GetOutput();

		if (PrevValue.size() != NextValue.size()) return false;
		if (PrevValue.size() != 4) return false;

		// glmのクォータニオンは wxyzで指定する必要がある？
		glm::quat PrevQuat = glm::quat(PrevValue[3], PrevValue[0], PrevValue[1], PrevValue[2]);
		glm::quat NextQuat = glm::quat(NextValue[3], NextValue[0], NextValue[1], NextValue[2]);

		glm::quat dstQuat = glm::slerp(PrevQuat, NextQuat, L);

		Value.push_back(dstQuat.x);
		Value.push_back(dstQuat.y);
		Value.push_back(dstQuat.z);
		Value.push_back(dstQuat.w);

		return true;
	}

	bool CAnimationSampler::DoModelMatrixLinearInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// 線形補完(Linear)
		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		float L = (CurrentTime - PrevTime) / (NextTime - PrevTime);

		const auto& PrevValue = PrevKeyFrame->GetOutput();
		const auto& NextValue = NextKeyFrame->GetOutput();

		if (PrevValue.size() != NextValue.size()) return false;

		if (PrevValue.size() != 16 || NextValue.size() != 16) return false;

		// それぞれのPos・Rotate・Scaleを取得
		glm::vec3 PrevPos = glm::vec3(0.0f);
		glm::quat PrevQuat = glm::quat();
		glm::vec3 PrevScale = glm::vec3(1.0f);
		
		glm::vec3 NextPos = glm::vec3(0.0f);
		glm::quat NextQuat = glm::quat();
		glm::vec3 NextScale = glm::vec3(1.0f);
		
		glm::vec3 DstPos = glm::vec3(0.0f);
		glm::quat DstQuat = glm::quat();
		glm::vec3 DstScale = glm::vec3(1.0f);

		{
			glm::mat4 mat = glm::mat4(1.0f);
			std::memcpy(&mat[0][0], &PrevValue[0], sizeof(float) * PrevValue.size());

			math::CTransform::CastModelMatrixToTransform(mat, PrevPos, PrevQuat, PrevScale);
		}
		
		{
			glm::mat4 mat = glm::mat4(1.0f);
			std::memcpy(&mat[0][0], &NextValue[0], sizeof(float) * NextValue.size());

			math::CTransform::CastModelMatrixToTransform(mat, NextPos, NextQuat, NextScale);
		}

		// それぞれを線形補完する
		// Pos
		{
			DstPos.x = (1.0f - L) * PrevPos.x + L * NextPos.x;
			DstPos.y = (1.0f - L) * PrevPos.y + L * NextPos.y;
			DstPos.z = (1.0f - L) * PrevPos.z + L * NextPos.z;
		}

		// Rotate
		{
			DstQuat = glm::slerp(PrevQuat, NextQuat, L);
			DstQuat = glm::normalize(DstQuat);
		}

		// Scale
		{
			/*DstScale.x = (1.0f - L) * PrevScale.x + L * NextScale.x;
			DstScale.y = (1.0f - L) * PrevScale.y + L * NextScale.y;
			DstScale.z = (1.0f - L) * PrevScale.z + L * NextScale.z;*/

			DstScale = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		// 補完結果はMatrixに戻さずにPos・Rotate・Scaleの順番でValueに格納する
		Value.push_back(DstPos.x); Value.push_back(DstPos.y); Value.push_back(DstPos.z);
		Value.push_back(DstQuat.x); Value.push_back(DstQuat.y); Value.push_back(DstQuat.z); Value.push_back(DstQuat.w); 
		Value.push_back(DstScale.x); Value.push_back(DstScale.y); Value.push_back(DstScale.z);

		return true;
	}

	bool CAnimationSampler::DoCubicSplineInterpolation(float CurrentTime, std::vector<float>& Value, EAnimationTarget AnimationTarget, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// CubicSpline: 3次スプライン曲線

		// rotationの場合、補完される四元数は正規化されている必要がある。
		// また、エクスポートの際に全ての要素が0の四元数を書き出さないようにも注意が必要。
		// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#interpolation-cubic

		// [メモ]
		// BlenderでなかなかCubic Spline形式で書き出すことができないのとglTF Sampler Modelsも全てLINEAR形式で
		// ネット上でも特にろくなサンプルが見当たらないので、CubicSplineInterpolationはそのようなモデルに出会ったときに実装対応する。

		return true;
	}
}