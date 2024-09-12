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

		int BytePerPixel = 0;
		int NumOfChannel = 0;

		// 実際のイメージデータ
		std::vector<unsigned char> compressed_data;

		while (!Analyser.IsEnd())
		{
			unsigned int ChunkLength = 0;
			if (!Analyser.GetUIntReverse(ChunkLength)) return false;

			std::string ChunkType = std::string();
			if (!Analyser.GetString(ChunkType, 4)) return false;

			if (ChunkType == "IHDR")
			{
				// 画像サイズ
				unsigned int w = 0, h = 0;

				if (!Analyser.GetUIntReverse(w)) return false;
				if (!Analyser.GetUIntReverse(h)) return false;

				Width = static_cast<int>(w);
				Height = static_cast<int>(h);

				// ビット深度(8が通常, グレースケール画像は8ビットが1つ, カラー画像は8ビットが4つが基本)
				unsigned char bit_depth = 0;
				if (!Analyser.GetByte(bit_depth)) return false;
				BytePerPixel = static_cast<int>(bit_depth) / 8;

				// カラータイプ(0: グレースケール画像, 6: RGBAカラー画像)
				unsigned char ColorType = 0;
				if (!Analyser.GetByte(ColorType)) return false;

				if (ColorType == 0)
				{
					NumOfChannel = 1;
				}
				else if (ColorType == 6)
				{
					NumOfChannel = 4; // RGBA
				}

				// 圧縮手法(使わないのでスキップ)
				if (!Analyser.Skip(1)) return false;

				// フィルター手法(使わないのでスキップ)
				if (!Analyser.Skip(1)) return false;

				// インターレース手法(使わないのでスキップ)
				if (!Analyser.Skip(1)) return false;

				// 8ビットグレースケール画像かRGBAカラー画像でない場合はエラーとする
				if (bit_depth != 8 || (ColorType != 0 && ColorType != 6))
				{
					Console::Log("[Error] PNGParser - InValid Image Format.\n");
					return false;
				}
			}
			else if (ChunkType == "IDAT")
			{
				// PNG形式に圧縮されている実際のイメージデータ
				idat_found = true;
				compressed_data.resize(ChunkLength);
				if (!Analyser.GetBinary(0, compressed_data, ChunkLength)) return false;
			}
			else if (ChunkType == "IEND")
			{
				// PNGの終わり
			}
			else
			{
				// その他チャンクは無視
				if (!Analyser.Skip(ChunkLength)) return false;
			}

			// チャンク末尾のCRC4バイト分スキップする
			if (!Analyser.Skip(4)) return false;
		}

		// zlibを使ってデータを解凍
		std::vector<uint8_t> decompressed_data(Width * Height * BytePerPixel * NumOfChannel + Height * 1);  // フィルタバイト込み(よくわからんが + Heightのこと？ → 後述の実装によるとピクセル列の先頭4バイトにフィルタタイプとかいうのが入っているらしい)
		uLongf decompressed_size = static_cast<uLongf>(decompressed_data.size());
		int result = uncompress(decompressed_data.data(), &decompressed_size, compressed_data.data(), static_cast<uLongf>(compressed_data.size()));
		decompressed_data.resize(decompressed_size);

		if (result != Z_OK)
		{
			Console::Log("[Error] PNGParser - ZLib Decompress Error.\n");
			return false;
		}

		// フィルタバイトを取り除きながらピクセルデータを取得
		// フィルタには対応しないのでこの部分を取り除く(無視)しながらピクセル取得を進める
		// エンジンとしては画像は常にカラー画像として取り扱っているので4倍してRGBA分のサイズを確保している
		outPixelData.resize(Width * Height * BytePerPixel * 4);

		binary::CBinaryReader PixelAnalyser(decompressed_data);

		for (int Row = 0; Row < Height; Row++)
		{
			// フィルタはスキップ(1Byteのフラグ)
			if (!PixelAnalyser.Skip(1)) return false;

			if (NumOfChannel == 1)
			{
				// エンジンとしては画像は常にカラー画像として取り扱っているのでグレースケール画像はカラー画像に変換する
				// 1列分コピー
				int ByteSize = Width * BytePerPixel * NumOfChannel;
				std::vector<unsigned char> CurrentPixelData(ByteSize);
				if (!PixelAnalyser.GetBinary(0, CurrentPixelData, ByteSize)) return false;

				//
				int RowStartPos = Width * Row * BytePerPixel * NumOfChannel * 4;
				for (int Col = 0; Col < ByteSize; Col++)
				{
					unsigned char data = CurrentPixelData[Col];

					outPixelData[RowStartPos + Col * 4 + 0] = data; // R
					outPixelData[RowStartPos + Col * 4 + 1] = data; // G
					outPixelData[RowStartPos + Col * 4 + 2] = data; // B
					outPixelData[RowStartPos + Col * 4 + 3] = 255;  // A
				}
			}
			else if (NumOfChannel == 4)
			{
				// カラー画像なのでそのままコピー
				if (!PixelAnalyser.GetBinary(Width * Row * BytePerPixel * NumOfChannel, outPixelData, Width * BytePerPixel * NumOfChannel)) return false;
			}
		}

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