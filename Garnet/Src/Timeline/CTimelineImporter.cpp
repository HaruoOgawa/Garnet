#ifdef USE_BINARY_READ
#include "CTimelineImporter.h"
#include "CTimelineClip.h"
#include "CNodeTrack.h"
#include "CMaterialTrack.h"
#include "../Binary/CBinaryReader.h"
#include "../Message/Console.h"

namespace timeline
{
	bool CTimelineImporter::Import(const std::vector<unsigned char>& Data, std::shared_ptr<CTimelineClip>& DstClip)
	{
		if (Data.empty())
		{
			Console::Log("[Timeline Import Error] Data is empty.\n");
			return false;
		}

		if (!DstClip)
		{
			Console::Log("[Timeline Import Error] DstClip is empty.\n");
			return false;
		}

		binary::CBinaryReader Reader = binary::CBinaryReader(Data);

		// ヘッダを読む(GLTL)
		std::string Head = std::string();
		if (!Reader.GetString(Head, 4)) return false;

		if (Head != "GTTL")
		{
			Console::Log("[Timeline Import Error] Invalid Head (%s)\n", Head.c_str());
			return false;
		}

		// バージョン
		int Version = -1;
		if (!Reader.GetInt(Version)) return false;
		
		if (Version < 0 || Version > 0)
		{
			Console::Log("[Timeline Import Error] Invalid Version (%d)\n", Version);
			return false;
		}

		// 最大再生時間
		float MaxTime = 0.0f;
		if (!Reader.GetFloat(MaxTime)) return false;

		DstClip->SetMaxTime(MaxTime);

		// トラック
		if (!ReadTrack(Reader, DstClip))
		{
			Console::Log("[Timeline Import Error] Failed to ReadTrack.\n");
			return false;
		}

		// サンプラー
		if (!ReadSampler(Reader, DstClip))
		{
			Console::Log("[Timeline Import Error] Failed to ReadSampler.\n");
			return false;
		}

		return true;
	}

	bool CTimelineImporter::ReadTrack(binary::CBinaryReader& Reader, std::shared_ptr<CTimelineClip>& DstClip)
	{
		// トラック数
		int NumOfTrack = 0;
		if (!Reader.GetInt(NumOfTrack)) return false;

		// トラックデータ
		for (int i = 0; i < NumOfTrack; i++)
		{
			// 共通パラメーター
			int TrackIDByteSize = 0;
			if (!Reader.GetInt(TrackIDByteSize)) return false;

			std::string TrackID = std::string();
			if (!Reader.GetString(TrackID, TrackIDByteSize)) return false;

			int SamplerIndex = -1;
			if (!Reader.GetInt(SamplerIndex)) return false;

			int SamplerTarget_Int = -1;
			if (!Reader.GetInt(SamplerTarget_Int)) return false;
			ETimelineSamplerTarget SamplerTarget = static_cast<ETimelineSamplerTarget>(SamplerTarget_Int);

			int ValueType_Int = -1;
			if (!Reader.GetInt(ValueType_Int)) return false;
			math::EValueType ValueType = static_cast<math::EValueType>(ValueType_Int);

			// トラックタイプ
			int TrackType_Int = -1;
			if(!Reader.GetInt(TrackType_Int)) return false;
			ETrackType TrackType = static_cast<ETrackType>(TrackType_Int);

			// 各トラックタイプごとの処理
			switch (TrackType)
			{
			case timeline::ETrackType::TrackType_None:
				break;
			case timeline::ETrackType::TrackType_Node:
				{
					int TrackTarget_Int = -1;
					if (!Reader.GetInt(TrackTarget_Int)) return false;
					ENodeTrackTarget TrackTarget = static_cast<ENodeTrackTarget>(TrackTarget_Int);

					// トラックを作成
					std::shared_ptr<CNodeTrack> Track = std::make_shared<CNodeTrack>(TrackID, SamplerIndex, SamplerTarget, TrackTarget);
					DstClip->AddTrack(Track);
				}
				break;
			case timeline::ETrackType::TrackType_Material:
				{
					int TrackTarget_Int = -1;
					if (!Reader.GetInt(TrackTarget_Int)) return false;
					EMaterialTrackTarget TrackTarget = static_cast<EMaterialTrackTarget>(TrackTarget_Int);

					int UniformNameByteSize = 0;
					if (!Reader.GetInt(UniformNameByteSize)) return false;

					std::string UniformName = std::string();
					if (!Reader.GetString(UniformName, UniformNameByteSize)) return false;

					// トラックを作成
					std::shared_ptr<CMaterialTrack> Track = std::make_shared<CMaterialTrack>(TrackID, SamplerIndex, SamplerTarget, TrackTarget, UniformName, ValueType);
					DstClip->AddTrack(Track);
				}
				break;
			default:
				break;
			}
		}

		return true;
	}

	bool CTimelineImporter::ReadSampler(binary::CBinaryReader& Reader, std::shared_ptr<CTimelineClip>& DstClip)
	{
		// サンプラー数
		int NumOfSampler = 0;
		if (!Reader.GetInt(NumOfSampler)) return false;

		// サンプラーデータ
		for (int SamplerIndex = 0; SamplerIndex < NumOfSampler; SamplerIndex++)
		{
			// 補間タイプ
			int InterpolationType_Int = -1;
			if (!Reader.GetInt(InterpolationType_Int)) return false;
			animation::EInterpolationType InterpolationType = static_cast<animation::EInterpolationType>(InterpolationType_Int);

			// サンプラーを作成
			std::shared_ptr<animation::CAnimationSampler> Sampler = std::make_shared<animation::CAnimationSampler>(InterpolationType);

			// キーフレーム数
			int NumOfKeyFrame = 0;
			if (!Reader.GetInt(NumOfKeyFrame)) return false;

			// キーフレームデータ
			for (int KeyFrameIndex = 0; KeyFrameIndex < NumOfKeyFrame; KeyFrameIndex++)
			{
				// キーフレームタイプ
				int KeyFrameType_Int = -1;
				if (!Reader.GetInt(KeyFrameType_Int)) return false;
				math::EValueType KeyFrameType = static_cast<math::EValueType>(KeyFrameType_Int);

				// インプット
				float Input = 0.0f;
				if (!Reader.GetFloat(Input)) return false;

				// アウトプット
				int NumOfComponent = math::CMath::GetNumComponentsInType(KeyFrameType);

				std::vector<float> Value;
				for (int n = 0; n < NumOfComponent; n++)
				{
					float v = 0.0f;
					if (!Reader.GetFloat(v)) return false;

					Value.push_back(v);
				}

				// キーフレームを作成
				std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(KeyFrameType);
				KeyFrame->SetInput(Input);
				KeyFrame->SetOutput(Value);

				Sampler->AddKeyFrame(KeyFrame);
			}

			// StartTime・EndTimeを計算
			Sampler->CalcStartEndTime();

			// サンプラーを追加する
			DstClip->AddSampler(Sampler);
		}

		return true;
	}
}
#endif