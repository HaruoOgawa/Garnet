#ifdef USE_MMD
#include "CVMDImporter.h"
#include "CVMDData.h"
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

		return true;
	}
}
#endif