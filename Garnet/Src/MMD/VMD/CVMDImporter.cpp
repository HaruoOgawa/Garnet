#ifdef USE_MMD
#include "CVMDImporter.h"
#include "../../Math/CTransform.h"
#include "../../Message/Console.h"
#include <algorithm>

namespace mmd
{
	bool CVMDImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		std::vector<std::shared_ptr<animation::CBlendShapeClip>>& BlendShapeClipList)
	{
		// (注意) MMDのボーンやキーフレームはワールド座標系を示すのでMMD以外のファイルフォーマットで使いまわすことはできない

		CVMDData vmd;

		// バイナリの解析
		if (!vmd.Analyse(pGraphicsAPI, Data))
		{
			Console::Log("[Cpp Log] Error - Failed to Analyse VMD Binary.\n");

			return false;
		}

		// アニメーションクリップの作成
		if (!CreateAnimationClip(vmd, AnimationClipList)) return false;

		// 表情アニメーションクリップの作成
		if (!CreateBlendShapeClip(vmd, BlendShapeClipList)) return false;

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
				// PMXにはスプライン補間しか存在しない
				std::shared_ptr<animation::CAnimationSampler> Sampler = std::make_shared<animation::CAnimationSampler>(animation::EInterpolationType::LINEAR);

				Sampler->SetStartTime(StartTime);
				Sampler->SetEndTime(EndTime);

				std::vector<SVMDFrame> FrameDataList = Frame.second;
				
				// FrameIndex順に並び替える
				std::sort(FrameDataList.begin(), FrameDataList.end(), [](SVMDFrame a, SVMDFrame b) {
					return a.FrameIndex < b.FrameIndex;
				});

				// KetFrame
				for (const auto& FrameData : FrameDataList)
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
					// MMDでは全てVALUE_TYPE_MATRIX
					{
						std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(math::EValueType::VALUE_TYPE_MATRIX);

						KeyFrame->SetInput(InputData);
						KeyFrame->SetOutput(OutputData);

						// スプライン補間用のポイントを渡す
						KeyFrame->SetXPointList(FrameData.XPointList);
						KeyFrame->SetYPointList(FrameData.YPointList);
						KeyFrame->SetZPointList(FrameData.ZPointList);
						KeyFrame->SetRPointList(FrameData.RPointList);

						// Add KeyFrame To Sampler
						Sampler->AddKeyFrame(KeyFrame);
					}

					// キーフレームが１つしかない時はEndTimeの位置にもう1つだけ追加する
					if (Frame.second.size() == 1)
					{
						std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(math::EValueType::VALUE_TYPE_MATRIX);

						KeyFrame->SetInput(EndTime);
						KeyFrame->SetOutput(OutputData);

						// スプライン補間用のポイントを渡す
						KeyFrame->SetXPointList(FrameData.XPointList);
						KeyFrame->SetYPointList(FrameData.YPointList);
						KeyFrame->SetZPointList(FrameData.ZPointList);
						KeyFrame->SetRPointList(FrameData.RPointList);

						// Add KeyFrame To Sampler
						Sampler->AddKeyFrame(KeyFrame);
					}
				}

				// SamplerをClipに登録する
				AnimationClip->AddAnimationSampler(Sampler);
			}

			// Channelを作成
			{
				// VMDはオフセットなので必要
				const bool UseAnimLocalAxis = true;

				// Translate成分がオフセットかどうか
				const bool IsTransOffset = true;

				// MMDでは全てVALUE_TYPE_MATRIX
				animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::MODELMATRIX;

				const int TargetSamplerIndex = Index;

				animation::EHumanoidBones BoneName = Frame.first;

				std::shared_ptr<animation::CAnimationChannel> Channel = std::make_shared<animation::CAnimationChannel>(UseAnimLocalAxis, IsTransOffset, TargetSamplerIndex, AnimationTarget, 
					std::string(), BoneName, nullptr);

				// ChannelをClipに登録
				AnimationClip->AddAnimationChannel(Channel);
			}

			Index++;
		}

		// クリップを登録
		AnimationClipList.push_back(AnimationClip);

		return true;
	}

	bool CVMDImporter::CreateBlendShapeClip(const CVMDData& VMDData, std::vector<std::shared_ptr<animation::CBlendShapeClip>>& BlendShapeClipList)
	{
		const auto& SkinFrameMap = VMDData.GetSkinFrameMap();

		// 0個の時は作成しない
		if (SkinFrameMap.size() == 0) return true;

		// アニメーションクリップを作成
		{
			// MMDのアニメーションは30FPSで固定
			const float FrameRate = 30.0f;

			int MinFrameIndex = VMDData.GetMinSkinFrameIndex();
			int MaxFrameIndex = VMDData.GetMaxSkinFrameIndex();

			const float StartTime = static_cast<float>(MinFrameIndex) * (1.0f / FrameRate);
			const float EndTime = static_cast<float>(MaxFrameIndex) * (1.0f / FrameRate);

			// CAnimationClipを継承したCBlendShapeAnimationClipがあってもいいかも？
			// CBlendShapeAnimationClip.UpdateでNodeのClearMorphWeightsを呼んであげる
			std::shared_ptr<animation::CBlendShapeClip> BlendShapeClip = std::make_shared<animation::CBlendShapeClip>();

			int Index = 0;

			for (const auto& Frame : SkinFrameMap)
			{
				// Samplerを作成
				{
					// PMXにはスプライン補間しか存在しない
					std::shared_ptr<animation::CAnimationSampler> Sampler = std::make_shared<animation::CAnimationSampler>(animation::EInterpolationType::LINEAR);

					Sampler->SetStartTime(StartTime);
					Sampler->SetEndTime(EndTime);

					std::vector<SVMDSkinFrame> FrameDataList = Frame.second;

					// FrameIndex順に並び替える
					std::sort(FrameDataList.begin(), FrameDataList.end(), [](SVMDSkinFrame a, SVMDSkinFrame b) {
						return a.FrameIndex < b.FrameIndex;
						});

					// KetFrame
					for (const auto& FrameData : FrameDataList)
					{
						int FrameIndex = FrameData.FrameIndex;
						float CurrentTime = static_cast<float>(FrameIndex) * (1.0f / FrameRate);

						float Weight = FrameData.Weight;

						// input
						float InputData = CurrentTime;

						// Output
						std::vector<float> OutputData;
						OutputData.push_back(Weight);

						// KeyFrameを作成
						{
							std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(math::EValueType::VALUE_TYPE_SCALAR);

							KeyFrame->SetInput(InputData);
							KeyFrame->SetOutput(OutputData);

							// Add KeyFrame To Sampler
							Sampler->AddKeyFrame(KeyFrame);
						}

						// キーフレームが１つしかない時はEndTimeの位置にもう1つだけ追加する
						if (Frame.second.size() == 1)
						{
							std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(math::EValueType::VALUE_TYPE_SCALAR);

							KeyFrame->SetInput(EndTime);
							KeyFrame->SetOutput(OutputData);

							// Add KeyFrame To Sampler
							Sampler->AddKeyFrame(KeyFrame);
						}
					}

					// SamplerをClipに登録する
					BlendShapeClip->AddAnimationSampler(Sampler);
				}

				// Channelを作成
				{
					// モーフなのでWEIGHTS
					animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::WEIGHTS;

					const int TargetSamplerIndex = Index;

					std::shared_ptr<animation::CAnimationChannel> Channel = std::make_shared<animation::CAnimationChannel>(false, false, TargetSamplerIndex, AnimationTarget, 
						std::string(), animation::EHumanoidBones::None, nullptr);

					// ChannelをClipに登録
					BlendShapeClip->AddAnimationChannel(Frame.first, Channel);
				}

				Index++;
			}

			// クリップを登録
			BlendShapeClipList.push_back(BlendShapeClip);
		}

		return true;
	}
}
#endif