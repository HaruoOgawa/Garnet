#ifdef USE_MMD
#include "CPmxModel.h"
#include "../../Binary/CBinaryAnalyser.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	bool CPmxModel::Analyse(const std::vector<unsigned char>& Data)
	{
		// Analyserを生成
		binary::CBinaryAnalyser Analyser(Data);

		// ヘッダが『PMX 』かどうか
		std::string header = "";
		if (!Analyser.GetString(header, 4)) return false;
		if (header != "PMX ")
		{
			Console::Log("[Error] Analyse PMX Error. Invalid header: %s\n", header.c_str());

			return false;
		}

		// Version
		if (Analyser.GetPointer()[0] != 0x00 || Analyser.GetPointer()[1] != 0x00 || Analyser.GetPointer()[2] != 0x00 || Analyser.GetPointer()[3] != 0x40) return false;
		if (!Analyser.Skip(4)) return false;

		// メタデータ
		SMetaData MetaData;
		if (!AnalyseMetaData(Analyser, MetaData))
		{
			Console::Log("[Error] Pmx AnalyseMetaData Error\n");

			return false;
		}

		// Mesh
		if (!AnalyseMesh(Analyser, MetaData))
		{
			Console::Log("[Error] Pmx AnalyseMesh Error\n");

			return false;
		}

		// Texture
		if (!AnalyseTexture(Analyser, MetaData))
		{
			Console::Log("[Error] Pmx AnalyseTexture Error\n");

			return false;
		}

		return true;
	}

	bool CPmxModel::AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SMetaData& MetaData)
	{
		// 後続のメタデータの長さ(PMX 2.0では8に固定)
		unsigned char MetaSize = 0;
		if (!Analyser.GetByte(MetaSize)) return false;

		if (!Analyser.IsValid(static_cast<size_t>(MetaSize))) return false;

		// メタデータを読む
		MetaData.EncodeType = static_cast<EEncodeType>(static_cast<int>(Analyser.GetByte())); // エンコード方式
		MetaData.AdditionalUVCount = static_cast<int>(Analyser.GetByte()); // 追加UV数
		MetaData.VertexIndexSize = static_cast<int>(Analyser.GetByte()); // 頂点インデックスサイズ
		MetaData.TextureIndexSize = static_cast<int>(Analyser.GetByte()); // テクスチャインデックスサイズ
		MetaData.MaterialIndexSize = static_cast<int>(Analyser.GetByte()); // マテリアルインデックスサイズ
		MetaData.BoneIndexSize = static_cast<int>(Analyser.GetByte()); // ボーンインデックスサイズ
		MetaData.MorphIndexSize = static_cast<int>(Analyser.GetByte()); // モーフインデックスサイズ
		MetaData.RigidIndexSize = static_cast<int>(Analyser.GetByte()); // 剛体インデックスサイズ

		// モデル名
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.ModelName.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.ModelName.first, ByteLength)) return false;
			}
		}

		// モデル名英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.ModelName_EN.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.ModelName_EN.first, ByteLength)) return false;
			}
		}

		// コメント
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.Comment.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.Comment.first, ByteLength)) return false;
			}
		}

		// コメント英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.Comment_EN.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.Comment_EN.first, ByteLength)) return false;
			}
		}

		return true;
	}

	bool CPmxModel::AnalyseMesh(binary::CBinaryAnalyser& Analyser, const SMetaData& MetaData)
	{
		// 頂点バッファの読み込み
		{
			int NumOfVertex = 0;
			if (!Analyser.GetInt(NumOfVertex)) return false;

			std::vector<float> PositionAttribute;
			std::vector<float> NormalAttribute;
			std::vector<float> UVAttribute;
			std::vector<float> TangentAttribute;
			
			// MetaData.BoneIndexSizeに応じてバイト数が変わる
			std::vector<int> IntJointAttribute;
			std::vector<unsigned char> ByteJointAttribute;
			std::vector<unsigned short> UShortJointAttribute;
			
			std::vector<float> WeightAttribute;

			std::vector<std::vector<float>> AdditionalUVAttribute;
			AdditionalUVAttribute.resize(MetaData.AdditionalUVCount);

			for (int VertexIndex = 0; VertexIndex < NumOfVertex; VertexIndex++)
			{
				// 位置(x, y, z)
				{
					if (!Analyser.IsValid(4 * 3)) return false;

					float x = Analyser.GetFloat();
					float y = Analyser.GetFloat();
					float z = Analyser.GetFloat();

					PositionAttribute.push_back(x);
					PositionAttribute.push_back(y);
					PositionAttribute.push_back(z);
				}

				// 法線(x, y, z)
				{
					if (!Analyser.IsValid(4 * 3)) return false;

					float x = Analyser.GetFloat();
					float y = Analyser.GetFloat();
					float z = Analyser.GetFloat();

					NormalAttribute.push_back(x);
					NormalAttribute.push_back(y);
					NormalAttribute.push_back(z);
				}

				// UV(u, v)
				{
					if (!Analyser.IsValid(4 * 2)) return false;

					float u = Analyser.GetFloat();
					float v = Analyser.GetFloat();

					UVAttribute.push_back(u);
					UVAttribute.push_back(v);
				}

				// Additional UV(x, y, z, w) * n
				for (int AddUVIndex = 0; AddUVIndex < MetaData.AdditionalUVCount; AddUVIndex++)
				{
					if (!Analyser.IsValid(4 * 4)) return false;

					float x = Analyser.GetFloat();
					float y = Analyser.GetFloat();
					float z = Analyser.GetFloat();
					float w = Analyser.GetFloat();

					AdditionalUVAttribute[AddUVIndex].push_back(x);
					AdditionalUVAttribute[AddUVIndex].push_back(y);
					AdditionalUVAttribute[AddUVIndex].push_back(z);
					AdditionalUVAttribute[AddUVIndex].push_back(w);
				}

				// Joints, Weghts
				{
					// ウェイト変形方式 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF
					unsigned char WeightFormatIndex = 0;
					if (!Analyser.GetByte(WeightFormatIndex)) return false;

					// Joints, Weghtsの格納方法
					EWeightDeformFormat WeightDeformFormat = static_cast<EWeightDeformFormat>(static_cast<int>(WeightFormatIndex));

					if (WeightDeformFormat == EWeightDeformFormat::BDEF1)
					{
						// BDEF1 : int 		| 4   | ボーンのみ
						/*
						n : ボーンIndexサイズ  | ウェイト1.0の単一ボーン(参照Index)
						*/
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						WeightAttribute.push_back(1.0f);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						
						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
					}
					else if (WeightDeformFormat == EWeightDeformFormat::BDEF2)
					{
						// BDEF2 : int,int,float 	| 4*3 | ボーン2つと、ボーン1のウェイト値(PMD方式)
						/*
						  n : ボーンIndexサイズ  | ボーン1の参照Index
						  n : ボーンIndexサイズ  | ボーン2の参照Index
						  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
						*/
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						if (!Analyser.IsValid(4 * 1)) return false;

						float WeightX = Analyser.GetFloat();
						float WeightY = 1.0f - WeightX;

						WeightAttribute.push_back(WeightX);
						WeightAttribute.push_back(WeightY);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
					}
					else if (WeightDeformFormat == EWeightDeformFormat::BDEF4)
					{
						// BDEF4 : int*4, float*4	| 4*8 | ボーン4つと、それぞれのウェイト値。ウェイト合計が1.0である保障はしない
						/*
						  n : ボーンIndexサイズ  | ボーン1の参照Index
						  n : ボーンIndexサイズ  | ボーン2の参照Index
						  n : ボーンIndexサイズ  | ボーン3の参照Index
						  n : ボーンIndexサイズ  | ボーン4の参照Index
						  4 : float              | ボーン1のウェイト値
						  4 : float              | ボーン2のウェイト値
						  4 : float              | ボーン3のウェイト値
						  4 : float              | ボーン4のウェイト値 (ウェイト計1.0の保障はない)
						*/
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						if (!Analyser.IsValid(4 * 4)) return false;

						float WeightX = Analyser.GetFloat();
						float WeightY = Analyser.GetFloat();
						float WeightZ = Analyser.GetFloat();
						float WeightW = Analyser.GetFloat();

						WeightAttribute.push_back(WeightX);
						WeightAttribute.push_back(WeightY);
						WeightAttribute.push_back(WeightZ);
						WeightAttribute.push_back(WeightW);
					}
					else if (WeightDeformFormat == EWeightDeformFormat::SDEF)
					{
						// SDEF  : int,int,float, float3*3 
						//			| 4*12 | BDEF2に加え、SDEF用のfloat3(Vector3)が3つ。実際の計算ではさらに補正値の算出が必要(一応そのままBDEF2としても使用可能)
						/*
						  n : ボーンIndexサイズ  | ボーン1の参照Index
						  n : ボーンIndexサイズ  | ボーン2の参照Index
						  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
						 12 : float3             | SDEF-C値(x,y,z)
						 12 : float3             | SDEF-R0値(x,y,z)
						 12 : float3             | SDEF-R1値(x,y,z) ※修正値を要計算
						*/
						
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						if (!Analyser.IsValid(4 * 10)) return false;

						float WeightX = Analyser.GetFloat();
						float WeightY = 1.0f - WeightX;

						WeightAttribute.push_back(WeightX);
						WeightAttribute.push_back(WeightY);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, IntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);

						// SDEF(未対応)
						float SDEF_C_X = Analyser.GetFloat();
						float SDEF_C_Y = Analyser.GetFloat();
						float SDEF_C_Z = Analyser.GetFloat();

						float SDEF_R0_X = Analyser.GetFloat();
						float SDEF_R0_Y = Analyser.GetFloat();
						float SDEF_R0_Z = Analyser.GetFloat();
						
						float SDEF_R1_X = Analyser.GetFloat();
						float SDEF_R1_Y = Analyser.GetFloat();
						float SDEF_R1_Z = Analyser.GetFloat();
					}
					else
					{
						return false;
					}
				}

				// EdgeScale
				float EdgeScale = 1.0f;
				if (!Analyser.GetFloat(EdgeScale)) return false;
			}
		}

		// インデックスバッファの読み込み
		{
			std::vector<int> IntIndices;
			std::vector<unsigned char> ByteIndices;
			std::vector<unsigned short> UShortIndices;

			int NumOfIndices = 0;
			if (!Analyser.GetInt(NumOfIndices)) return false;
			
			const int VertexIndexSize = MetaData.VertexIndexSize;

			for (int i = 0; i < NumOfIndices; i++)
			{
				if (!GetMultiTypeValue(Analyser, MetaData.VertexIndexSize, IntIndices, ByteIndices, UShortIndices)) return false;
			}
		}

		return true;
	}

	bool CPmxModel::AnalyseTexture(binary::CBinaryAnalyser& Analyser, const SMetaData& MetaData)
	{
		int NumOfTexture = 0;
		if (!Analyser.GetInt(NumOfTexture)) return false;

		std::vector<std::pair<std::string, std::wstring>> TextureNameList;

		for (int TextureIndex = 0; TextureIndex < NumOfTexture; TextureIndex++)
		{
			std::pair<std::string, std::wstring> TextureName = std::make_pair(std::string(), std::wstring());

			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(TextureName.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EEncodeType::UTF8)
			{
				if (!Analyser.GetString(TextureName.first, ByteLength)) return false;
			}

			TextureNameList.push_back(TextureName);

			Console::Log("TextureName: %ls\n", TextureName.second.c_str());
		}

		return true;
	}

	// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CPmxModel::GetMultiTypeValue(binary::CBinaryAnalyser& Analyser, int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList)
	{
		if (ByteSize == 1)
		{
			unsigned char JointIndex = 0;
			if (!Analyser.GetByte(JointIndex)) return false;

			ByteValueList.push_back(JointIndex);
		}
		else if (ByteSize == 2)
		{
			unsigned short JointIndex = 0;
			if (!Analyser.GetUShort(JointIndex)) return false;

			UShortValueList.push_back(JointIndex);
		}
		else if (ByteSize == 4)
		{
			int JointIndex = 0;
			if (!Analyser.GetInt(JointIndex)) return false;

			IntValueList.push_back(JointIndex);
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CPmxModel::AddEmptyMultiTypeValue(int ByteSize, std::vector<int>& IntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList)
	{
		if (ByteSize == 1)
		{
			unsigned char JointIndex = 0;

			ByteValueList.push_back(JointIndex);
		}
		else if (ByteSize == 2)
		{
			unsigned short JointIndex = 0;

			UShortValueList.push_back(JointIndex);
		}
		else if (ByteSize == 4)
		{
			int JointIndex = 0;

			IntValueList.push_back(JointIndex);
		}
		else
		{
			return false;
		}

		return true;
	}
}
#endif