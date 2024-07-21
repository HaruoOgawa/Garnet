#ifdef USE_BINARY_WRITE
#include "CTimelineExporter.h"
#include "CTimelineClip.h"
#include "../Binary/CBinaryWriter.h"
#include "../Message/Console.h"
#include "../LoadWorker/CFile.h"

namespace timeline
{
	bool CTimelineExporter::Export(const std::string& FileName, const std::shared_ptr<CTimelineClip>& Clip)
	{
		if (FileName.empty() || !Clip)
		{
			Console::Log("[Timeline Export Error] FileName is empty\n");
			return false;
		}

		if (!Clip)
		{
			Console::Log("[Timeline Export Error] Clip is empty\n");
			return false;
		}

		binary::CBinaryWriter Writer;

		// ヘッダ書き込み
		Writer.SetString("GTTL");

		// バージョン書き込み
		Writer.SetInt(0);

		// 最大再生時間
		Writer.SetFloat(Clip->GetMaxTime());

		// トラック書き込み
		if (!WriteTrack(Writer, Clip)) return false;

		// サンプラー書き込み
		if (!WriteSampler(Writer, Clip)) return false;

		// ファイル出力
		resource::CFile File = resource::CFile(FileName);
		File.SetData(Writer.GetData());
		if (!File.Write()) return false;

		return true;
	}

	bool CTimelineExporter::WriteTrack(binary::CBinaryWriter& Writer, const std::shared_ptr<CTimelineClip>& Clip)
	{
		const auto& TrackClist = Clip->GetTrackList();

		// トラック数
		Writer.SetInt(static_cast<int>(TrackClist.size()));

		// トラックデータ
		for (const auto& Track : TrackClist)
		{
			Writer.SetString(Track.second->GetTrackID());
			Writer.SetInt(Track.second->GetSamplerIndex());
			Writer.SetInt(static_cast<int>(Track.second->GetSamplerTarget()));
			Writer.SetInt(static_cast<int>(Track.second->GetValueType()));
		}

		return true;
	}

	bool CTimelineExporter::WriteSampler(binary::CBinaryWriter& Writer, const std::shared_ptr<CTimelineClip>& Clip)
	{
		const auto& SamplerList = Clip->GetSamplerList();

		// サンプラー数
		Writer.SetInt(static_cast<int>(SamplerList.size()));

		// サンプラーデータ
		for (const auto& Sampler : SamplerList)
		{
			const auto& KeyFrameList = Sampler->GetKeyFrameList();

			// 補間タイプ
			Writer.SetInt(static_cast<int>(Sampler->GetInterpolationType()));

			// キーフレーム数
			Writer.SetInt(static_cast<int>(KeyFrameList.size()));

			// キーフレームデータ
			for (const auto& KeyFrame : KeyFrameList)
			{
				Writer.SetInt(static_cast<int>(KeyFrame->GetType()));

				Writer.SetFloat(KeyFrame->GetInput());

				for (float v : KeyFrame->GetOutput())
				{
					Writer.SetFloat(v);
				}
			}
		}

		return true;
	}
}
#endif