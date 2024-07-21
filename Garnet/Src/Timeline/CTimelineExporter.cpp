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

		// 最大再生時間
		Writer.SetFloat(Clip->GetMaxTime());

		// トラック書き込み

		// サンプラー書き込み

		// ファイル出力
		resource::CFile File = resource::CFile(FileName);
		File.SetData(Writer.GetData());
		if (!File.Write()) return false;

		return true;
	}
}
#endif