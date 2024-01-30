#ifdef USE_MMD
#include "CVMDImporter.h"
#include "../../Math/CTransform.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	bool CVMDImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		// (注意) MMDのボーンやキーフレームはワールド座標系を示すのでMMD以外のファイルフォーマットで使いまわすことはできない

		CVMDData vmd;

		// バイナリの解析
		if (!vmd.Analyse(Data))
		{
			Console::Log("[Cpp Log] Error - Failed to Analyse VMD Binary.\n");

			return false;
		}

		// アニメーションクリップの作成
		if (!CreateAnimationClip(vmd, AnimationClipList)) return false;

		return true;
	}

	bool CVMDImporter::CreateAnimationClip(const CVMDData& VMDData, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		// MMDのアニメーションは30FPSで固定
		const float FrameRate = 30.0f;

		int MinFrameIndex = VMDData.GetMinFrameIndex();
		int MaxFrameIndex = VMDData.GetMaxFrameIndex();

		const float StartTime = static_cast<float>(MinFrameIndex) * (1.0f / FrameRate);
		const float EndTime = static_cast<float>(MaxFrameIndex) * (1.0f / FrameRate);

		//
		std::shared_ptr<animation::CAnimationClip> AnimationClip = std::make_shared<animation::CAnimationClip>();

		int Index = 0;

		for (const auto& Frame : VMDData.GetFrameMap())
		{
			// Samplerを作成
			{
				// MMDにはCubicSplineしかないがひとまずLINEARで試しに作ってみる
				std::shared_ptr<animation::CAnimationSampler> Sampler = std::make_shared<animation::CAnimationSampler>(animation::EInterpolationType::LINEAR);

				Sampler->SetStartTime(StartTime);
				Sampler->SetEndTime(EndTime);

				// KetFrame
				for (const auto& FrameData : Frame.second)
				{
					int FrameIndex = FrameData.FrameIndex;
					float CurrentTime = static_cast<float>(FrameIndex) * (1.0f / FrameRate);

					const auto& Pos = FrameData.Pos;
					const auto& Rot = FrameData.Rot;

					glm::mat4 CurrentMatrix = glm::mat4(1.0f);
					math::CTransform::CalcModelMatrix(CurrentMatrix, Pos, Rot, false);

					// input
					float InputData = CurrentTime;

					// Output
					std::vector<float> OutputData(16);
					std::memcpy(&OutputData[0], &CurrentMatrix[0][0], sizeof(glm::mat4));

					// KeyFrameを作成
					// MMDでは全てKEYFRAME_TYPE_MATRIX
					std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(animation::EKeyFrameType::KEYFRAME_TYPE_MATRIX);

					KeyFrame->SetInput(InputData);
					KeyFrame->SetOutput(OutputData);

					// Add KeyFrame To Sampler
					Sampler->AddKeyFrame(KeyFrame);
				}

				// SamplerをClipに登録する
				AnimationClip->AddAnimationSampler(Sampler);
			}

			// Channelを作成
			{
				// どうするか保留
				const bool UseAnimLocalAxis = false;

				// MMDでは全てKEYFRAME_TYPE_MATRIX
				animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::MODELMATRIX;

				const int TargetSamplerIndex = Index;

				animation::EHumanoidBones BoneName = Frame.first;

				std::shared_ptr<animation::CAnimationChannel> Channel = std::make_shared<animation::CAnimationChannel>(UseAnimLocalAxis, TargetSamplerIndex, AnimationTarget, nullptr, BoneName);

				// ChannelをClipに登録
				AnimationClip->AddAnimationChannel(Channel);
			}

			Index++;
		}

		// クリップを登録
		AnimationClipList.push_back(AnimationClip);

		return true;
	}
}
#endif