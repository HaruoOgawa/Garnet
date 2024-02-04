#ifdef USE_MMD
#include "CPmxModel.h"
#include "../../Binary/CBinaryAnalyser.h"
#include "../../Debug/Message/Console.h"

namespace mmd
{
	CPmxModel::CPmxModel():
		m_PmxMesh(nullptr)
	{
	}

	CPmxModel::~CPmxModel()
	{
	}

	const SPmxMetaData& CPmxModel::GetMetaData() const
	{
		return m_MetaData;
	}

	const std::shared_ptr<CPmxMesh>& CPmxModel::GetPmxMesh() const
	{
		return m_PmxMesh;
	}

	const std::vector<std::shared_ptr<CPmxTexture>>& CPmxModel::GetPmxTextureList() const
	{
		return m_PmxTextureList;
	}

	const std::vector<std::shared_ptr<CPmxMaterial>>& CPmxModel::GetPmxMaterialList() const
	{
		return m_PmxMaterialList;
	}

	const std::vector<std::shared_ptr<CPmxBone>>& CPmxModel::GetPmxBoneList() const
	{
		return m_PmxBoneList;
	}

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
		if (!AnalyseMetaData(Analyser, m_MetaData))
		{
			Console::Log("[Error] Pmx AnalyseMetaData Error\n");

			return false;
		}

		// Mesh
		if (!AnalyseMesh(Analyser, m_MetaData))
		{
			Console::Log("[Error] Pmx AnalyseMesh Error\n");

			return false;
		}

		// Texture
		if (!AnalyseTexture(Analyser, m_MetaData))
		{
			Console::Log("[Error] Pmx AnalyseTexture Error\n");

			return false;
		}

		// Material
		if (!AnalyseMaterial(Analyser, m_MetaData))
		{
			Console::Log("[Error] Pmx AnalyseMaterial Error\n");

			return false;
		}

		// Bone
		if (!AnalyseBone(Analyser, m_MetaData))
		{
			Console::Log("[Error] Pmx AnalyseBone Error\n");

			return false;
		}

		return true;
	}

	bool CPmxModel::AnalyseMetaData(binary::CBinaryAnalyser& Analyser, SPmxMetaData& MetaData)
	{
		// 後続のメタデータの長さ(PMX 2.0では8に固定)
		unsigned char MetaSize = 0;
		if (!Analyser.GetByte(MetaSize)) return false;

		if (!Analyser.IsValid(static_cast<size_t>(MetaSize))) return false;

		// メタデータを読む
		MetaData.EncodeType = static_cast<EPmxEncodeType>(static_cast<int>(Analyser.GetByte())); // エンコード方式
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

			if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.ModelName.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.ModelName.first, ByteLength)) return false;
			}
		}

		// モデル名英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.ModelName_EN.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.ModelName_EN.first, ByteLength)) return false;
			}
		}

		// コメント
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.Comment.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.Comment.first, ByteLength)) return false;
			}
		}

		// コメント英
		{
			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(MetaData.Comment_EN.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				if (!Analyser.GetString(MetaData.Comment_EN.first, ByteLength)) return false;
			}
		}

		return true;
	}

	bool CPmxModel::AnalyseMesh(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData)
	{
		// 頂点バッファの読み込み
		std::vector<float> PositionAttribute;
		std::vector<float> NormalAttribute;
		std::vector<float> UVAttribute;
		std::vector<float> TangentAttribute;

		// MetaData.BoneIndexSizeに応じてバイト数が変わる
		std::vector<unsigned int> UIntJointAttribute;
		std::vector<unsigned char> ByteJointAttribute;
		std::vector<unsigned short> UShortJointAttribute;

		std::vector<float> WeightAttribute;

		std::vector<std::vector<float>> AdditionalUVAttribute;
		AdditionalUVAttribute.resize(MetaData.AdditionalUVCount);

		{
			int NumOfVertex = 0;
			if (!Analyser.GetInt(NumOfVertex)) return false;

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
					EPmxWeightDeformFormat WeightDeformFormat = static_cast<EPmxWeightDeformFormat>(static_cast<int>(WeightFormatIndex));

					if (WeightDeformFormat == EPmxWeightDeformFormat::BDEF1)
					{
						// BDEF1 : int 		| 4   | ボーンのみ
						/*
						n : ボーンIndexサイズ  | ウェイト1.0の単一ボーン(参照Index)
						*/
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						WeightAttribute.push_back(1.0f);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						
						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
					}
					else if (WeightDeformFormat == EPmxWeightDeformFormat::BDEF2)
					{
						// BDEF2 : int,int,float 	| 4*3 | ボーン2つと、ボーン1のウェイト値(PMD方式)
						/*
						  n : ボーンIndexサイズ  | ボーン1の参照Index
						  n : ボーンIndexサイズ  | ボーン2の参照Index
						  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
						*/
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						if (!Analyser.IsValid(4 * 1)) return false;

						float WeightX = Analyser.GetFloat();
						float WeightY = 1.0f - WeightX;

						WeightAttribute.push_back(WeightX);
						WeightAttribute.push_back(WeightY);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						WeightAttribute.push_back(0.0f);
						WeightAttribute.push_back(0.0f);
					}
					else if (WeightDeformFormat == EPmxWeightDeformFormat::BDEF4)
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
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

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
					else if (WeightDeformFormat == EPmxWeightDeformFormat::SDEF)
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
						
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!GetMultiTypeValue(Analyser, MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

						if (!Analyser.IsValid(4 * 10)) return false;

						float WeightX = Analyser.GetFloat();
						float WeightY = 1.0f - WeightX;

						WeightAttribute.push_back(WeightX);
						WeightAttribute.push_back(WeightY);

						// あまりは0埋めする
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;
						if (!AddEmptyMultiTypeValue(MetaData.BoneIndexSize, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute)) return false;

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
		std::vector<unsigned int> UIntIndices;
		std::vector<unsigned char> ByteIndices;
		std::vector<unsigned short> UShortIndices;

		{
			int NumOfIndices = 0;
			if (!Analyser.GetInt(NumOfIndices)) return false;
			
			const int VertexIndexSize = MetaData.VertexIndexSize;

			for (int i = 0; i < NumOfIndices; i++)
			{
				if (!GetMultiTypeValue(Analyser, MetaData.VertexIndexSize, UIntIndices, ByteIndices, UShortIndices)) return false;
			}
		}

		m_PmxMesh = std::make_shared<CPmxMesh>(PositionAttribute, NormalAttribute, UVAttribute, TangentAttribute, UIntJointAttribute, ByteJointAttribute, UShortJointAttribute, WeightAttribute, AdditionalUVAttribute, UIntIndices, ByteIndices, UShortIndices);

		return true;
	}

	bool CPmxModel::AnalyseTexture(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData)
	{
		int NumOfTexture = 0;
		if (!Analyser.GetInt(NumOfTexture)) return false;

		for (int TextureIndex = 0; TextureIndex < NumOfTexture; TextureIndex++)
		{
			std::shared_ptr<CPmxTexture> PmxTexture = std::make_shared<CPmxTexture>();

			std::pair<std::string, std::wstring> TextureName = std::make_pair(std::string(), std::wstring());

			int ByteLength = 0;
			if (!Analyser.GetInt(ByteLength)) return false;

			if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				if (!Analyser.GetUTF16String(TextureName.second, ByteLength)) return false;
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				if (!Analyser.GetString(TextureName.first, ByteLength)) return false;
			}

			PmxTexture->SetFilePath(TextureName);

			m_PmxTextureList.push_back(PmxTexture);

			Console::Log("TextureName: %ls\n", TextureName.second.c_str());
		}

		return true;
	}

	bool CPmxModel::AnalyseMaterial(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData)
	{
		int NumOfMaterial = 0;
		if (!Analyser.GetInt(NumOfMaterial)) return false;

		for (int MaterialIndex = 0; MaterialIndex < NumOfMaterial; MaterialIndex++)
		{
			// MaterialName
			std::pair<std::string, std::wstring> MaterialName = std::make_pair(std::string(""), std::wstring(L""));
			{
				int ByteLength = 0;
				if (!Analyser.GetInt(ByteLength)) return false;

				if (MetaData.EncodeType == EPmxEncodeType::UTF8)
				{
					if (!Analyser.GetString(MaterialName.first, ByteLength)) return false;
				}
				else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
				{
					if (!Analyser.GetUTF16String(MaterialName.second, ByteLength)) return false;
				}
			}

			// MaterialName_EN
			std::pair<std::string, std::wstring> MaterialName_EN = std::make_pair(std::string(""), std::wstring(L""));
			{
				int ByteLength = 0;
				if (!Analyser.GetInt(ByteLength)) return false;

				if (MetaData.EncodeType == EPmxEncodeType::UTF8)
				{
					if (!Analyser.GetString(MaterialName_EN.first, ByteLength)) return false;
				}
				else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
				{
					if (!Analyser.GetUTF16String(MaterialName_EN.second, ByteLength)) return false;
				}
			}

			// Diffuse
			glm::vec4 Diffuse = glm::vec4(1.0f);
			{
				if (!Analyser.IsValid(4 * 4)) return false;

				float R = Analyser.GetFloat();
				float G = Analyser.GetFloat();
				float B = Analyser.GetFloat();
				float A = Analyser.GetFloat();

				Diffuse = glm::vec4(R, G, B, A);
			}

			// Specular
			glm::vec4 Specular = glm::vec4(0.0f);
			{
				if (!Analyser.IsValid(4 * 3)) return false;

				float R = Analyser.GetFloat();
				float G = Analyser.GetFloat();
				float B = Analyser.GetFloat();

				Specular = glm::vec4(R, G, B, 1.0f);
			}

			// Specular係数
			float SpecularCoef = 1.0f;
			if (!Analyser.GetFloat(SpecularCoef)) return false;

			// Ambient
			glm::vec4 Ambient = glm::vec4(0.0f);
			{
				if (!Analyser.IsValid(4 * 3)) return false;

				float R = Analyser.GetFloat();
				float G = Analyser.GetFloat();
				float B = Analyser.GetFloat();

				Ambient = glm::vec4(R, G, B, 1.0f);
			}

			/*
			bitFlag  	| 描画フラグ(8bit) - 各bit 0:OFF 1:ON
                   0x01:両面描画, 0x02:地面影, 0x04:セルフシャドウマップへの描画, 0x08:セルフシャドウの描画, 
                   0x10:エッジ描画
			// CPmaxMaterial内で解析する
			*/
			unsigned char DrawBitFlag = 0;
			if (!Analyser.GetByte(DrawBitFlag)) return false;

			// エッジカラー
			glm::vec4 EdgeColor = glm::vec4(0.0f);
			{
				if (!Analyser.IsValid(4 * 4)) return false;

				float R = Analyser.GetFloat();
				float G = Analyser.GetFloat();
				float B = Analyser.GetFloat();
				float A = Analyser.GetFloat();

				EdgeColor = glm::vec4(R, G, B, A);
			}

			// エッジサイズ
			float EdgeSize = 1.0f;
			if (!Analyser.GetFloat(EdgeSize)) return false;

			// メインテクスチャの参照インデックス
			int MainTexIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.TextureIndexSize);

			// スフィアテクスチャの参照インデックス
			int SphereTexIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.TextureIndexSize);

			// スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
			unsigned char SphereModeByte = 0;
			if (!Analyser.GetByte(SphereModeByte)) return false;

			EPmxSphereMode SphereMode = static_cast<EPmxSphereMode>(static_cast<int>(SphereModeByte));

			// 共有Toonフラグ 0:継続値は個別Toon 1:継続値は共有Toon
			unsigned char SharedToonFlag = 0;
			if (!Analyser.GetByte(SharedToonFlag)) return false;

			// トゥーンテクスチャ
			int ToonTexIndex = -1;
			int SharedToonTexIndex = -1;

			if (SharedToonFlag == 0)
			{
				// Toonテクスチャ, テクスチャテーブルの参照Index
				ToonTexIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.TextureIndexSize);
			}
			else if (SharedToonFlag == 1)
			{
				// 共有Toonテクスチャ[0～9] -> それぞれ toon01.bmp～toon10.bmp に対応
				unsigned char SharedToonTexByte = 0;
				if (!Analyser.GetByte(SharedToonTexByte)) return false;

				SharedToonTexIndex = static_cast<int>(SharedToonTexByte);
			}
			else
			{
				return false;
			}

			// メモ : 自由欄／スクリプト記述／エフェクトへのパラメータ配置など
			std::pair<std::string, std::wstring> MaterialDescription = std::make_pair(std::string(""), std::wstring(L""));
			{
				int ByteLength = 0;
				if (!Analyser.GetInt(ByteLength)) return false;

				if (MetaData.EncodeType == EPmxEncodeType::UTF8)
				{
					if (!Analyser.GetString(MaterialDescription.first, ByteLength)) return false;
				}
				else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
				{
					if (!Analyser.GetUTF16String(MaterialDescription.second, ByteLength)) return false;
				}
			}

			// 材質に対応する面(頂点)数 (必ず3の倍数になる)
			int MatRefIndiceCount = 0;
			if (!Analyser.GetInt(MatRefIndiceCount)) return false;

			// マテリアルを登録
			std::shared_ptr<CPmxMaterial> PmxMaterial = std::make_shared<CPmxMaterial>(MaterialName, MaterialName_EN, Diffuse, Specular, SpecularCoef, Ambient, DrawBitFlag, EdgeColor, EdgeSize, 
				MainTexIndex, SphereTexIndex, SphereMode, ToonTexIndex, SharedToonTexIndex, MaterialDescription, MatRefIndiceCount);

			m_PmxMaterialList.push_back(PmxMaterial);
		}

		return true;
	}

	bool CPmxModel::AnalyseBone(binary::CBinaryAnalyser& Analyser, const SPmxMetaData& MetaData)
	{
		int NumOfBone = 0;
		if (!Analyser.GetInt(NumOfBone)) return false;

		for (int BoneIndex = 0; BoneIndex < NumOfBone; BoneIndex++)
		{
			// BoneName
			std::pair<std::string, std::wstring> BoneName = std::make_pair(std::string(""), std::wstring(L""));
			{
				int ByteLength = 0;
				if (!Analyser.GetInt(ByteLength)) return false;

				if (MetaData.EncodeType == EPmxEncodeType::UTF8)
				{
					if (!Analyser.GetString(BoneName.first, ByteLength)) return false;
				}
				else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
				{
					if (!Analyser.GetUTF16String(BoneName.second, ByteLength)) return false;
				}
			}

			// BoneName_EN
			std::pair<std::string, std::wstring> BoneName_EN = std::make_pair(std::string(""), std::wstring(L""));
			{
				int ByteLength = 0;
				if (!Analyser.GetInt(ByteLength)) return false;

				if (MetaData.EncodeType == EPmxEncodeType::UTF8)
				{
					if (!Analyser.GetString(BoneName_EN.first, ByteLength)) return false;
				}
				else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
				{
					if (!Analyser.GetUTF16String(BoneName_EN.second, ByteLength)) return false;
				}
			}

			// 位置
			glm::vec3 Pos = glm::vec3(0.0f);
			{
				if (!Analyser.IsValid(4 * 3)) return false;

				Pos.x = Analyser.GetFloat();
				Pos.y = Analyser.GetFloat();
				Pos.z = Analyser.GetFloat();
			}

			// 親ボーンのインデックス
			int ParentBoneIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.BoneIndexSize);

			// 変形階層
			int DeformLayer = -1;
			if (!Analyser.GetInt(DeformLayer)) return false;

			// ボーンフラグ(16bit)
			unsigned short BoneFlag = 0;
			if (!Analyser.GetUShort(BoneFlag)) return false;

			// PmxBoneを作成
			std::shared_ptr<CPmxBone> PmxBone = std::make_shared<CPmxBone>(BoneName, BoneName_EN, Pos, ParentBoneIndex, DeformLayer, BoneFlag);

			// ボーンフラグを見て処理を分ける
			{
				// 接続先
				if (BoneFlag & 0x0001)
				{
					// 接続先: 1
					// 接続先ボーンのボーンIndex(ネットで調べるときは『表示先』と出る)
					int ConnectBoneIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.BoneIndexSize);
				}
				else
				{
					// 接続先: 0
					// 座標オフセット, ボーン位置からの相対分
					if (!Analyser.IsValid(4 * 3)) return false;

					glm::vec3 Offset = glm::vec3(0.0f);

					Offset.x = Analyser.GetFloat();
					Offset.y = Analyser.GetFloat();
					Offset.z = Analyser.GetFloat();

					// Posにオフセットを追加する
					Pos += Offset;
				}

				// 回転付与 または 移動付与 が 1
				if (BoneFlag & 0x0100 || BoneFlag & 0x0200)
				{
					// 付与親ボーンのボーンIndex
					int GrantParentBoneIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.BoneIndexSize);

					// 付与率
					float GrantRate = 0.0f;
					if (!Analyser.GetFloat(GrantRate)) return false;
				}

				// 軸固定:1 の場合
				if (BoneFlag & 0x0400)
				{
					if (!Analyser.IsValid(4 * 3)) return false;

					glm::vec3 FixedAxisVector = glm::vec3(0.0f);

					FixedAxisVector.x = Analyser.GetFloat();
					FixedAxisVector.y = Analyser.GetFloat();
					FixedAxisVector.z = Analyser.GetFloat();
				}

				// ローカル軸:1 の場合
				if (BoneFlag & 0x0800)
				{
					if (!Analyser.IsValid(4 * 3 * 2)) return false;

					//
					glm::vec3 XAxisVector = glm::vec3(0.0f);

					XAxisVector.x = Analyser.GetFloat();
					XAxisVector.y = Analyser.GetFloat();
					XAxisVector.z = Analyser.GetFloat();

					XAxisVector = glm::normalize(XAxisVector);

					//
					glm::vec3 ZAxisVector = glm::vec3(0.0f);

					ZAxisVector.x = Analyser.GetFloat();
					ZAxisVector.y = Analyser.GetFloat();
					ZAxisVector.z = Analyser.GetFloat();

					ZAxisVector = glm::normalize(ZAxisVector);

					//
					PmxBone->SetLocalAxis(XAxisVector, ZAxisVector);
				}

				// 外部親変形:1 の場合
				if (BoneFlag & 0x2000)
				{
					int KeyIndex = -1;
					if (!Analyser.GetInt(KeyIndex)) return false;
				}

				// IK:1 の場合 IKデータを格納
				if (BoneFlag & 0x0020)
				{
					// IKターゲットボーンのボーンIndex
					int IKTargetBoneIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.BoneIndexSize);

					// IKループ回数 (PMD及びMMD環境では255回が最大になるようです)
					int IKLoopCount = 0;
					if (!Analyser.GetInt(IKLoopCount)) return false;

					// IKループ計算時の1回あたりの制限角度 -> ラジアン角 | PMDのIK値とは4倍異なるので注意
					float LimitedAngle = 0.0f;
					if (!Analyser.GetFloat(LimitedAngle)) return false;

					// IKリンク数 : 後続の要素数
					int IKLinkCount = 0;
					if (!Analyser.GetInt(IKLinkCount)) return false;

					std::vector<animation::SIKLink> IKLinkList;

					for (int IKLinkIndex = 0; IKLinkIndex < IKLinkCount; IKLinkIndex++)
					{
						// リンクボーンのボーンIndex
						int IKLinkBoneIndex = GetMultiTypeValueAsInterger(Analyser, MetaData.BoneIndexSize);

						// 角度制限 0:OFF 1:ON
						unsigned char IsLimitAngle = 0;
						if (!Analyser.GetByte(IsLimitAngle)) return false;

						bool UseLimitAngle = false;
						glm::vec3 LowerAngle = glm::vec3(0.0f);
						glm::vec3 UpperAngle = glm::vec3(0.0f);

						if (IsLimitAngle & 0x01)
						{
							if (!Analyser.IsValid(4 * 3 * 2)) return false;

							UseLimitAngle = true;

							LowerAngle.x = Analyser.GetFloat();
							LowerAngle.y = Analyser.GetFloat();
							LowerAngle.z = Analyser.GetFloat();

							UpperAngle.x = Analyser.GetFloat();
							UpperAngle.y = Analyser.GetFloat();
							UpperAngle.z = Analyser.GetFloat();
						}

						// IKLinkを登録
						IKLinkList.push_back(animation::SIKLink{ IKLinkBoneIndex , UseLimitAngle ,LowerAngle, UpperAngle });
					}

					// IKParamを登録
					std::shared_ptr<animation::SIKParam> IKParam = std::make_shared<animation::SIKParam>(IKTargetBoneIndex, IKLoopCount, LimitedAngle, IKLinkList);

					PmxBone->SetIKParam(IKParam);
				}
			}

			// PmxBoneを登録
			m_PmxBoneList.push_back(PmxBone);
		}

		return true;
	}

	// Helper Functions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CPmxModel::GetMultiTypeValue(binary::CBinaryAnalyser& Analyser, int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList)
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

			UIntValueList.push_back(static_cast<unsigned int>(JointIndex));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CPmxModel::AddEmptyMultiTypeValue(int ByteSize, std::vector<unsigned int>& UIntValueList, std::vector<unsigned char>& ByteValueList, std::vector<unsigned short>& UShortValueList)
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
			unsigned int JointIndex = 0;

			UIntValueList.push_back(JointIndex);
		}
		else
		{
			return false;
		}

		return true;
	}

	int CPmxModel::GetMultiTypeValueAsInterger(binary::CBinaryAnalyser& Analyser, int ByteSize)
	{
		int Result = -1;

		if (ByteSize == 1)
		{
			unsigned char Index = 0;
			if (!Analyser.GetByte(Index)) return -1;

			Result = static_cast<int>(Index);
		}
		else if (ByteSize == 2)
		{
			unsigned short Index = 0;
			if (!Analyser.GetUShort(Index)) return -1;

			Result = static_cast<int>(Index);
		}
		else if (ByteSize == 4)
		{
			int Index = 0;
			if (!Analyser.GetInt(Index)) return -1;

			Result = Index;
		}
		else
		{
			Result = -1;
		}

		return Result;
	}
}
#endif