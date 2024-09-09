#ifdef USE_PNG_PARSER
#include "CPNGParserer.h"
#include <zlib.h>
#include "../Binary/CBinaryReader.h"
#include "../Message/Console.h"

namespace image_parse
{
	bool CPNGParserer::Parse(const std::vector<unsigned char>& Data, std::vector<unsigned char>& outPixelData, int& Width, int& Height)
	{
		// Analyserを生成
		binary::CBinaryReader Analyser(Data);

		// PNGヘッダかどうか
		if (!CheckPNGHeader(Analyser)) 
		{
			Console::Log("[Error] PNGParserer - InValid PNG Header\n");
			return false;
		}

		// PNGのバイナリはヘッダと数種類のチャンクから構成される
		// チャンクはブロックみたいな塊で『Length(4バイト), Chunk Type(4バイト), Chunk Data(0バイト以上), CRC(4バイト)』の4要素で構成される
		// https://qiita.com/spc_ehara/items/c748ec636283df805926
		bool idat_found = false; // idatチャンクを見つけたか

		while (!Analyser.IsEnd())
		{
			unsigned int ChunkLength = 0;
			if (!Analyser.GetUIntReverse(ChunkLength)) return false;

			std::string ChunkType = std::string();
			if (!Analyser.GetString(ChunkType, 4)) return false;

			/*if (ChunkType == "IHDR")
			{

			}
			else if (ChunkType == "IDAT")
			{

			}
			else if (ChunkType == "IEND")
			{
				// PNGの終わり
				break;
			}
			else*/
			{
				// その他チャンクは無視
				if (!Analyser.Skip(ChunkLength)) return false;
			}

			// チャンク末尾のCRC4バイト分スキップする
			if (!Analyser.Skip(4)) return false;
		}

		// zlibを使ってデータを解凍
		std::vector<uint8_t> compressed_data;
		std::vector<uint8_t> decompressed_data;  // フィルタバイト込み
		
		uLongf decompressed_size = decompressed_data.size();
		int result = uncompress(decompressed_data.data(), &decompressed_size, compressed_data.data(), compressed_data.size());

		return true;
	}

	bool CPNGParserer::CheckPNGHeader(binary::CBinaryReader& Analyser)
	{
		if (!Analyser.IsValid(8)) return false;

		if (Analyser.GetByte() != 0x89 || Analyser.GetByte() != 0x50 || Analyser.GetByte() != 0x4E || Analyser.GetByte() != 0x47 ||
			Analyser.GetByte() != 0x0D || Analyser.GetByte() != 0x0A || Analyser.GetByte() != 0x1A || Analyser.GetByte() != 0x0A) return false;

		return true;
	}
}
#endif