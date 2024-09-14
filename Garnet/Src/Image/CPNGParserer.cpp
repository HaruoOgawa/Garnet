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
		int BPP = 0; // ピクセルあたりのバイト数。NumOfChannelでもいい。RGBAカラーは4, グレースケールは1

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
					BPP = 1;
				}
				else if (ColorType == 6)
				{
					BPP = 4; // RGBA
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
		std::vector<uint8_t> decompressed_data(Width * Height * BytePerPixel * BPP + Height * 1);  // フィルタバイト込み
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

		std::vector<unsigned char> PrevScanline;

		for (int Row = 0; Row < Height; Row++)
		{
			// フィルタータイプ
			unsigned char FilterType = 0;
			if (!PixelAnalyser.GetByte(FilterType)) return false;

			// 1列分コピー
			int ByteSize = Width * BytePerPixel * BPP;
			std::vector<unsigned char> Scanline(ByteSize);
			if (!PixelAnalyser.GetBinary(0, Scanline, ByteSize)) return false;

			std::vector<unsigned char> CurrentPixelData(ByteSize);

			// フィルタリングを解除して元のピクセルデータを復元する
			if (!UnfilterScanline(CurrentPixelData, Scanline, PrevScanline, FilterType, BPP)) return false;

			// 次のフィルタリング解除のために1つ前のデータを保持しておく
			PrevScanline = CurrentPixelData;

			// ピクセルデータを保存
			int ByteOffset = Width * Row * BytePerPixel * BPP;
			if (BPP == 1)
			{
				// エンジンとしては画像は常にカラー画像として取り扱っているのでグレースケール画像はカラー画像に変換する
				int RowStartPos = ByteOffset * 4;
				for (int Col = 0; Col < ByteSize; Col++)
				{
					unsigned char data = CurrentPixelData[Col];

					outPixelData[RowStartPos + Col * 4 + 0] = data; // R
					outPixelData[RowStartPos + Col * 4 + 1] = data; // G
					outPixelData[RowStartPos + Col * 4 + 2] = data; // B
					outPixelData[RowStartPos + Col * 4 + 3] = 255;  // A
				}
			}
			else if (BPP == 4)
			{
				// カラー画像なのでそのままコピー
				std::memcpy(&outPixelData[ByteOffset], &CurrentPixelData[0], ByteSize);
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

	bool CPNGParserer::UnfilterScanline(std::vector<unsigned char>& Recon, const std::vector<unsigned char>& Scanline, const std::vector<unsigned char>& PrevScanline, unsigned char FilterType, int BPP)
	{
		switch (FilterType)
		{
		case 0: // Noneフィルタ : 前処理は行わない
			{
				for (size_t i = 0; i < Scanline.size(); ++i)
				{
					Recon[i] = Scanline[i];
				}
			}
			break;

		case 1: // Subフィルタ : 左隣との差分を取る
			{
				for (size_t i = 0; i < BPP; ++i)
				{
					Recon[i] = Scanline[i];  // 最初のBPPバイトはそのまま
				}

				for (size_t i = BPP; i < Scanline.size(); ++i)
				{
					Recon[i] = Scanline[i] + Recon[i - BPP];
				}
			}
			break;

		case 2: // Upフィルタ : 真上との差分を取る
			{
				if (PrevScanline.empty()) 
				{
					for (size_t i = 0; i < Scanline.size(); ++i)
					{
						Recon[i] = Scanline[i];
					}
				}
				else 
				{
					for (size_t i = 0; i < Scanline.size(); ++i)
					{
						Recon[i] = Scanline[i] + PrevScanline[i];
					}
				}
			}
			break;

		case 3: // Averageフィルタ : 左隣と真上の成分を足して2で割った値との差分を取る
			{
				for (size_t i = 0; i < BPP; ++i) 
				{
					Recon[i] = Scanline[i] + (PrevScanline.empty() ? 0 : PrevScanline[i]) / 2;
				}
				for (size_t i = BPP; i < Scanline.size(); ++i) 
				{
					Recon[i] = Scanline[i] + ((Recon[i - BPP] + (PrevScanline.empty() ? 0 : PrevScanline[i])) / 2);
				}
			}
		break;

		case 4: // Paethフィルタ : 左隣、真上、左上の中で最も値が近いものとの差分を取る
			{
				for (size_t i = 0; i < BPP; ++i) 
				{
					Recon[i] = Scanline[i] + (PrevScanline.empty() ? 0 : PrevScanline[i]);
				}

				for (size_t i = BPP; i < Scanline.size(); ++i) 
				{
					Recon[i] = Scanline[i] + PaethPredictor(Recon[i - BPP], PrevScanline.empty() ? 0 : PrevScanline[i], PrevScanline.empty() ? 0 : PrevScanline[i - BPP]);
				}
			}
		break;

		default:
			break;
		}

		return true;
	}

	// Paethフィルタのヘルパー関数
	int CPNGParserer::PaethPredictor(int a, int b, int c)
	{
		int p = a + b - c;
		int pa = abs(p - a);
		int pb = abs(p - b);
		int pc = abs(p - c);
		if (pa <= pb && pa <= pc) return a;
		else if (pb <= pc) return b;
		else return c;
	}
}
#endif