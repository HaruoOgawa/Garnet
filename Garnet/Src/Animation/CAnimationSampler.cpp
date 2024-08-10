#include "CAnimationSampler.h"
#include "../Math/CTransform.h"
#include "../../Message/Console.h"
#include <algorithm>

namespace animation
{
	CAnimationSampler::CAnimationSampler(EInterpolationType InterpolationType):
		m_InterpolationType(InterpolationType),
		m_StartTime(0.0f),
		m_EndTime(0.0f),
		m_SelfSamplerIndex(-1)
	{
	}

	CAnimationSampler::~CAnimationSampler()
	{
	}

	EInterpolationType CAnimationSampler::GetInterpolationType() const
	{
		return m_InterpolationType;
	}

	bool CAnimationSampler::CreateKeyFrame(math::EValueType Type, const std::vector<float>& inputList, const std::vector<float>& outputList)
	{
		// 0の時はエラーにはしないが、何も処理しない
		if (inputList.size() == 0) return true;

		//if (inputList.size() != outputList.size() / NumComponent) return false;

		int NumComponent = static_cast<int>(outputList.size() / inputList.size());

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

		CalcStartEndTime();
		 
		return true;
	}

	void CAnimationSampler::AddKeyFrame(const std::shared_ptr<animation::CKeyFrame>& KeyFrame)
	{
		m_KeyFrameList.push_back(KeyFrame);

		CalcStartEndTime();
	}

	void CAnimationSampler::AddKeyFrameWithSort(const std::shared_ptr<animation::CKeyFrame>& KeyFrame)
	{
		m_KeyFrameList.push_back(KeyFrame);

		std::sort(m_KeyFrameList.begin(), m_KeyFrameList.end(), [](const auto& a, const auto& b) { return (a->GetInput() < b->GetInput()); });

		CalcStartEndTime();
	}

	void CAnimationSampler::RemoveKeyFrame(const std::shared_ptr<animation::CKeyFrame>& KeyFrame)
	{
		const auto it = std::find(m_KeyFrameList.begin(), m_KeyFrameList.end(), KeyFrame);
		if (it == m_KeyFrameList.end()) return;
		
		m_KeyFrameList.erase(it);

		CalcStartEndTime();
	}

	const std::vector<std::shared_ptr<animation::CKeyFrame>>& CAnimationSampler::GetKeyFrameList() const
	{
		return m_KeyFrameList;
	}

	std::vector<std::shared_ptr<animation::CKeyFrame>> CAnimationSampler::GetKeyFrameListFromRange(float FirstTime, float SecondTime)
	{
		std::vector<std::shared_ptr<animation::CKeyFrame>> dstKeyFrameList;

		for (const auto& KeyFrame : m_KeyFrameList)
		{
			// 小さいものはスキップ
			if (KeyFrame->GetInput() < FirstTime) continue;

			// 大きかったらそれ以上のキーフレームは入らないので探査終了
			if (KeyFrame->GetInput() >= SecondTime) break;

			// 追加する
			dstKeyFrameList.push_back(KeyFrame);
		}

		return dstKeyFrameList;
	}

	void CAnimationSampler::SetKeyFrameInput(const std::shared_ptr<animation::CKeyFrame>& KeyFrame, float NewInput)
	{
		// 既存キーフレームのInputを更新してソートする
		const auto it = std::find(m_KeyFrameList.begin(), m_KeyFrameList.end(), KeyFrame);
		if (it == m_KeyFrameList.end()) return;
		
		(*it)->SetInput(NewInput);

		std::sort(m_KeyFrameList.begin(), m_KeyFrameList.end(), [](const auto& a, const auto& b) { return (a->GetInput() < b->GetInput()); });

		CalcStartEndTime();
	}

	void CAnimationSampler::CalcStartEndTime()
	{
		if (m_KeyFrameList.size() > 0)
		{
			m_StartTime = m_KeyFrameList[0]->GetInput();
			m_EndTime = m_KeyFrameList[m_KeyFrameList.size() - 1]->GetInput();
		}
	}

	void CAnimationSampler::SetStartTime(float StartTime)
	{
		m_StartTime = StartTime;
	}

	float CAnimationSampler::GetStartTime() const
	{
		return m_StartTime;
	}

	void CAnimationSampler::SetEndTime(float EndTime)
	{
		m_EndTime = EndTime;
	}

	float CAnimationSampler::GetEndTime() const
	{
		return m_EndTime;
	}

	void CAnimationSampler::SetSelfSamplerIndex(int Index)
	{
		m_SelfSamplerIndex = Index;
	}

	bool CAnimationSampler::IsEnd(float CurrentTime)
	{
		return (CurrentTime > m_EndTime);
	}

	std::vector<float> CAnimationSampler::CopyFromNumComponent(int NumComponent, const std::vector<float>& Src, int Offset)
	{
		std::vector<float> Dst;

		Dst.resize(NumComponent);
		std::memcpy(&Dst[0], &Src[Offset], NumComponent * sizeof(float));

		return Dst;
	}

	bool CAnimationSampler::ComputeCurrentFrame(float CurrentTime, bool IsLoop, std::vector<float>& Value, EInterpolateValueType ValueType)
	{
		// 0の時はエラーにはしないが、何も処理しない
		// AnimationやSDKに使っていないボーンのアニメーションでもなぜか一つだけInput・Outputが入っていることがあるため
		if (m_KeyFrameList.empty()) return true;

		// 2つ以上ある時は必ずStartとEndの計算が必要
		if (m_StartTime >= m_EndTime && m_KeyFrameList.size() >= 2)
		{
			Console::Log("[Error - KeyFrame] StartTime is greater than EndTime. / StartTime: %f, EndTime: %f\n", m_StartTime, m_EndTime);
			return false;
		}

		float CalcCurrentTime = 0.0f;

		if (IsLoop)
		{
			CalcCurrentTime = glm::mod(CurrentTime, m_EndTime);
		}
		else
		{
			CalcCurrentTime = fminf(CurrentTime, m_EndTime);
		}

		// 処理対処のキーフレームを取得
		std::shared_ptr<animation::CKeyFrame> PrevKeyFrame = nullptr;
		std::shared_ptr<animation::CKeyFrame> NextKeyFrame = nullptr;
		
		if (!GetNeedKeyFrame(CalcCurrentTime, PrevKeyFrame, NextKeyFrame)) return false;

		// キーフレームが同じなら補間せずにPrevKeyFrameの値をそのまま返す
		if (PrevKeyFrame == NextKeyFrame)
		{
			Value = PrevKeyFrame->GetOutput();
			return true;
		}

		// 補完されたVakueを取得
		switch (m_InterpolationType)
		{
		case animation::EInterpolationType::STEP:
			if (!DoStepInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			break;
		case animation::EInterpolationType::LINEAR:
			{
				switch (ValueType)
				{
				case animation::EInterpolateValueType::NONE:
					if (!DoLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				case animation::EInterpolateValueType::QUATERNION:
					// 回転のLinearの場合、Slerp( Spherical Linear Interpolation)を使用する必要がある
					// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#appendix-c-interpolation
					if (!DoSphericalLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				case animation::EInterpolateValueType::MODELMATRIX:
					if (!DoModelMatrixLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				default:
					if (!DoLinearInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
					break;
				}
			}
			break;
		case animation::EInterpolationType::CUBICSPLINE:
		{
			if (ValueType == animation::EInterpolateValueType::MODELMATRIX)
			{
				if (!DoModelMatrixSplineInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			}
			else
			{
				if (!DoCubicSplineInterpolation(CalcCurrentTime, Value, PrevKeyFrame, NextKeyFrame)) return false;
			}
		}
			break;
		default:
			break;
		}

		return true;
	}

	bool CAnimationSampler::GetNeedKeyFrame(float CurrentTime, std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		if (m_KeyFrameList.empty()) return false;

		// Next
		const auto& val = std::find_if(m_KeyFrameList.begin(), m_KeyFrameList.end(), [&](std::shared_ptr<CKeyFrame>& f) {  bool r = (CurrentTime <= f->GetInput()); if (r) { NextKeyFrame = f; } return r; });
		
		// 次のフレームがない
		if (val == m_KeyFrameList.end())
		{
			if (m_KeyFrameList.size() < 2)
			{
				// 1つしかない時は最初のフレームを割り当てる
				NextKeyFrame = m_KeyFrameList[0];
				PrevKeyFrame = m_KeyFrameList[0];
			}
			else
			{
				// 最後のフレームを返す
				NextKeyFrame = m_KeyFrameList[m_KeyFrameList.size() - 1];
				PrevKeyFrame = m_KeyFrameList[m_KeyFrameList.size() - 1];
			}

			return true;
		}

		// Prev
		size_t NextIndex = std::distance(m_KeyFrameList.begin(), val);

		// CurrentTimeがKeyFrameの最初よりも小さい時はPrevとNextにそれぞれ同じキーフレームを割り当てる
		if (NextIndex <= 0 || NextIndex >= m_KeyFrameList.size())
		{
			NextKeyFrame = m_KeyFrameList[0];
			PrevKeyFrame = m_KeyFrameList[0];
		}
		else
		{
			PrevKeyFrame = m_KeyFrameList[NextIndex - 1];
		}
		
		if (PrevKeyFrame == nullptr || NextKeyFrame == nullptr) return false;

		return true;
	}

	float CAnimationSampler::GetInterpolateValue(float CurrentTime, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		float L = (CurrentTime - PrevTime) / (NextTime - PrevTime);

		// 最終フレームIDがクリップ全体のフレーム数よりも小さい時があり、補間係数が爆発することがあるので0から1にクランプする
		L = glm::clamp(L, 0.0f, 1.0f);

		return L;
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

		// 補間係数を計算
		const float L = GetInterpolateValue(CurrentTime, PrevKeyFrame, NextKeyFrame);

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

		// 補間係数を計算
		const float L = GetInterpolateValue(CurrentTime, PrevKeyFrame, NextKeyFrame);

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
		
		// 補間係数を計算
		const float L = GetInterpolateValue(CurrentTime, PrevKeyFrame, NextKeyFrame);

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
		}

		// Scale
		{
			DstScale.x = (1.0f - L) * PrevScale.x + L * NextScale.x;
			DstScale.y = (1.0f - L) * PrevScale.y + L * NextScale.y;
			DstScale.z = (1.0f - L) * PrevScale.z + L * NextScale.z;
		}

		// 補完結果はMatrixに戻さずにPos・Rotate・Scaleの順番でValueに格納する
		Value.push_back(DstPos.x); Value.push_back(DstPos.y); Value.push_back(DstPos.z);
		Value.push_back(DstQuat.x); Value.push_back(DstQuat.y); Value.push_back(DstQuat.z); Value.push_back(DstQuat.w); 
		Value.push_back(DstScale.x); Value.push_back(DstScale.y); Value.push_back(DstScale.z);

		return true;
	}

	bool CAnimationSampler::DoCubicSplineInterpolation(float CurrentTime, std::vector<float>& Value, const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
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

	bool CAnimationSampler::DoModelMatrixSplineInterpolation(float CurrentTime, std::vector<float>& Value, 
		const std::shared_ptr<animation::CKeyFrame>& PrevKeyFrame, const std::shared_ptr<animation::CKeyFrame>& NextKeyFrame)
	{
		// ModelMatrix
		// CubicSpline: 3次スプライン曲線

		float PrevTime = PrevKeyFrame->GetInput();
		float NextTime = NextKeyFrame->GetInput();

		// 補間係数を計算
		const float L = GetInterpolateValue(CurrentTime, PrevKeyFrame, NextKeyFrame);

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

		// それぞれでスプライン補間する
		// Pos
		{
			// X
			{
				const auto& Points = PrevKeyFrame->GetXPointList();
				if (Points.size() != 4) return false;

				glm::vec2 SplinePoint = CalculateSplinePont(L, Points[0], Points[1], Points[2], Points[3]);

				DstPos.x = (1.0f - SplinePoint.x) * PrevPos.x + SplinePoint.x * NextPos.x;
			}

			// Y
			{
				const auto& Points = PrevKeyFrame->GetYPointList();
				if (Points.size() != 4) return false;

				glm::vec2 SplinePoint = CalculateSplinePont(L, Points[0], Points[1], Points[2], Points[3]);

				DstPos.y = (1.0f - SplinePoint.x) * PrevPos.y + SplinePoint.x * NextPos.y;
			}
			
			// Z
			{
				const auto& Points = PrevKeyFrame->GetZPointList();
				if (Points.size() != 4) return false;

				glm::vec2 SplinePoint = CalculateSplinePont(L, Points[0], Points[1], Points[2], Points[3]);

				DstPos.z = (1.0f - SplinePoint.x) * PrevPos.z + SplinePoint.x * NextPos.z;
			}
		}

		// Rotate
		{
			const auto& Points = PrevKeyFrame->GetRPointList();
			if (Points.size() != 4) return false;

			glm::vec2 SplinePoint = CalculateSplinePont(L, Points[0], Points[1], Points[2], Points[3]);

			DstQuat = glm::slerp(PrevQuat, NextQuat, SplinePoint.x);
		}

		// Scale
		{
			DstScale.x = (1.0f - L) * PrevScale.x + L * NextScale.x;
			DstScale.y = (1.0f - L) * PrevScale.y + L * NextScale.y;
			DstScale.z = (1.0f - L) * PrevScale.z + L * NextScale.z;
		}

		// 補完結果はMatrixに戻さずにPos・Rotate・Scaleの順番でValueに格納する
		Value.push_back(DstPos.x); Value.push_back(DstPos.y); Value.push_back(DstPos.z);
		Value.push_back(DstQuat.x); Value.push_back(DstQuat.y); Value.push_back(DstQuat.z); Value.push_back(DstQuat.w);
		Value.push_back(DstScale.x); Value.push_back(DstScale.y); Value.push_back(DstScale.z);

		return true;
	}

	glm::vec2 CAnimationSampler::CalculateSplinePont(float t, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3)
	{
		float t2 = t * t;
		float t3 = t2 * t;

		float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t) / 6.0f;
		float b1 = (3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f;
		float b2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f;
		float b3 = t3 / 6.0f;

		glm::vec2 result = glm::vec2(0.0f);

		result.x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
		result.y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;

		return result;
	}

	// ToDo: どこも参照していなかったら後で消す
	/*// ボーンに基づく現在のフレームを取得
	std::shared_ptr<CKeyFrame> CAnimationSampler::GetCurrentKeyFrameBasedBone(float CurrentTime, EHumanoidBones BoneName, const std::unordered_map<animation::EHumanoidBones, std::vector<std::shared_ptr<animation::CKeyFrame>>>& FrameMatrixMap)
	{
		std::shared_ptr<CKeyFrame> dstKeyFrame = nullptr;

		auto FrameBonePair = FrameMatrixMap.find(BoneName);
		if (FrameBonePair == FrameMatrixMap.end()) return nullptr;

		std::shared_ptr<animation::CKeyFrame> PrevKeyFrame = nullptr;
		std::shared_ptr<animation::CKeyFrame> NextKeyFrame = nullptr;

		// Next
		const auto& val = std::find_if(FrameBonePair->second.begin(), FrameBonePair->second.end(), [&](std::shared_ptr<animation::CKeyFrame> f) {  bool r = (CurrentTime <= f->GetInput()); if (r) { NextKeyFrame = f; } return r; });
		if (val == FrameBonePair->second.end()) return nullptr;

		// Prev
		size_t NextIndex = std::distance(FrameBonePair->second.begin(), val);

		// CurrentTimeがKeyFrameの最初よりも小さい時はPrevとNextにそれぞれ0と1のKeyFrameを割り当てる
		if (NextIndex <= 0 || NextIndex >= FrameBonePair->second.size())
		{
			NextKeyFrame = FrameBonePair->second[1];
			PrevKeyFrame = FrameBonePair->second[0];
		}
		else
		{
			PrevKeyFrame = FrameBonePair->second[NextIndex - 1];
		}

		if (PrevKeyFrame == nullptr || NextKeyFrame == nullptr) return nullptr;;

		float NextOffset = glm::abs(NextKeyFrame->GetInput() - CurrentTime);
		float PrevOffset = glm::abs(PrevKeyFrame->GetInput() - CurrentTime);

		if (PrevOffset < NextOffset)
		{
			dstKeyFrame = PrevKeyFrame;
		}
		else
		{
			dstKeyFrame = NextKeyFrame;
		}

		return dstKeyFrame;
	}*/
}