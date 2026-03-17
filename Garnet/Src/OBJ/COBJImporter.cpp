#include "COBJImporter.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include "../../Graphics/CMaterialFrame.h"
#include "../../Graphics/CVertexBuffer.h"
#include "../../Graphics/CIndexBuffer.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../Binary/CBinaryReader.h"
#include "../../Format/CStringFormatter.h"
#ifdef _DEBUG
#include "../../Message/Console.h"
#endif // _DEBUG

namespace obj
{
	bool COBJImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
		const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
		resource::C3DObjectLoader* p3DObjectLoader)
	{
		if (!p3DObjectLoader) return false;

		binary::CBinaryReader Analyzer(Data);

		std::vector<std::string> LineList;

		std::vector<float> Positions;  // Dimention: 3
		std::vector<float> Texcoords; // Dimention: 2
		std::vector<float> Normals; // Dimention: 3

		std::map<std::string, std::vector<std::string>> VertexDataIndexes;

		std::string CurrentMeshName = std::string();

		// データをパース
		for (;;)
		{
			if (Analyzer.IsEnd()) break;

			std::string CurrentLine = std::string();
			if (!Analyzer.GetStringToLineBreak(CurrentLine)) return false;
			LineList.push_back(CurrentLine);

			std::vector<std::string> ParamList = format::CStringFormatter::Sprit(CurrentLine, ' ');
			if (ParamList.empty()) continue;

			Analyze(ParamList, CurrentLine, BaseDir, Object, p3DObjectLoader, CurrentMeshName, Positions, Texcoords, Normals, VertexDataIndexes);
		}

		// Objectデータ構築
		if (!Build(pGraphicsAPI, Object, BaseMaterialFrameList, Positions, Texcoords, Normals, VertexDataIndexes)) return false;

		return true;
	}

	void COBJImporter::Analyze(const std::vector<std::string>& ParamList, const std::string& CurrentLine,
		const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
		resource::C3DObjectLoader* p3DObjectLoader, std::string& CurrentMeshName,
		std::vector<float>& Positions, std::vector<float>& Texcoords, std::vector<float>& Normals,
		std::map<std::string, std::vector<std::string>>& VertexDataIndexes)
	{
		if (ParamList[0] == "v" && ParamList.size() >= 4)
		{
			// Positions
			Positions.push_back(std::stof(ParamList[1]));
			Positions.push_back(std::stof(ParamList[2]));
			Positions.push_back(std::stof(ParamList[3]));
		}
		else if (ParamList[0] == "vt" && ParamList.size() >= 4)
		{
			// uv
			Texcoords.push_back(std::stof(ParamList[1]));
			Texcoords.push_back(std::stof(ParamList[2]));
		}
		else if (ParamList[0] == "vn" && ParamList.size() >= 4)
		{
			// normal
			Normals.push_back(std::stof(ParamList[1]));
			Normals.push_back(std::stof(ParamList[2]));
			Normals.push_back(std::stof(ParamList[3]));
		}
		else if (ParamList[0] == "f" && ParamList.size() >= 4)
		{
			// face(頂点インデックス)
			VertexDataIndexes[CurrentMeshName].push_back(ParamList[1]);
			VertexDataIndexes[CurrentMeshName].push_back(ParamList[2]);
			VertexDataIndexes[CurrentMeshName].push_back(ParamList[3]);
		}
		else if (ParamList[0] == "usemtl" && ParamList.size() >= 2)
		{
			// メッシュ名兼マテリアル名
			CurrentMeshName = ParamList[1];
		}
		else if (ParamList[0] == "mtllib" && ParamList.size() >= 2)
		{
			// mtllibファイル名
			std::string mtllibName = ParamList[1];
			std::string fullPathMtl = BaseDir + "/" + mtllibName;

			std::vector<std::shared_ptr<graphics::CMaterialFrame>> BaseMaterialFrameList;
			std::vector<std::string> defaultmaterialframes;
			std::map<animation::EHumanoidBones, std::string> HumanoidBoneList;

			std::shared_ptr<resource::C3DObjectLoader> mtllibResource = std::make_shared<resource::C3DObjectLoader>(
				fullPathMtl, Object, BaseMaterialFrameList, defaultmaterialframes, animation::ERigType::None, HumanoidBoneList, 0
			);

			p3DObjectLoader->AddSubResource(mtllibResource);
		}
		else if (ParamList[0] == "#")
		{
			// コメントなので何もしない
			return;
		}
		else
		{
#ifdef _DEBUG
			Console::Log("[COBJImporter::Analyze] Non Supported Type Found. (%s)\n", CurrentLine.c_str());
#endif // _DEBUG
		}
	}

	bool COBJImporter::Build(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
		const std::vector<float>& SrcPositions, const std::vector<float>& SrcTexcoords, const std::vector<float>& SrcNormals,
		const std::map<std::string, std::vector<std::string>>& SrcVertexDataIndexes)
	{
		const bool ExistPosition = !SrcPositions.empty();
		const bool ExistTexcoord = !SrcTexcoords.empty();
		const bool ExistNormal   = !SrcNormals.empty();

		// Positionは必須
		if (!ExistPosition) return false;

		const int NumOfVertex = (static_cast<int>(SrcPositions.size()) / 3);
		const int NumOfPolygon = NumOfVertex / 3;

		for (const auto& NameIndexPair : SrcVertexDataIndexes)
		{
			const auto& MeshName = NameIndexPair.first;
			const auto& DataIndexes = NameIndexPair.second;

			int MeshIndex = static_cast<int>(Object->GetMeshList().size());
			int SelfNodeIndex = static_cast<int>(Object->GetNodeList().size());

			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			// ノード生成
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, SelfNodeIndex);
			Node->SetName(MeshName);
			Object->AddNode(Node);

			// 頂点バッファ・インデックスバッファ生成
			std::vector<float> ResultPositions;
			std::vector<float> ResultTexcoords;
			std::vector<float> ResultNormals;

			if (!ExistNormal)
			{
				ResultNormals.resize(NumOfVertex * 3, 0.0f);
			}

			// メッシュを作成する
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			// 頂点バッファ本体
			std::vector<std::vector<float>> VertexDataList;
			std::vector<int> DimentionList;
			std::vector<graphics::EDataType> DataTypeList;
			std::vector<int> ByteStrideList;

			std::vector<unsigned short> Indices;
			std::vector<unsigned int> UINTIndices;

			// 頂点データの初期化用(例えばWeightとかNormalを持っていないならそれを0埋めするみたいな処理)
			std::vector<std::string> NeedAttribNameList = {
				"POSITION",
				"NORMAL",
				"TEXCOORD_0",
				"TANGENT",
				"JOINTS_0",
				"WEIGHTS_0",
			};

			std::map<std::string, std::vector<float>> ReservedVertexDataList;
			std::map<std::string, graphics::EDataType> ReservedDataTypeList;
			std::map<std::string, int> ReservedByteStrideList;

			// OBJは常にタンジェントの計算が必要
			const bool NeedRecalculateTangent = true;

			//
			for (int v = 0; v < DataIndexes.size(); v += 3)
			{
				std::tuple<int, int, int> DataStructure_0 = GetDataStructure(DataIndexes[v + 0], ExistPosition, ExistTexcoord, ExistNormal);
				std::tuple<int, int, int> DataStructure_1 = GetDataStructure(DataIndexes[v + 1], ExistPosition, ExistTexcoord, ExistNormal);
				std::tuple<int, int, int> DataStructure_2 = GetDataStructure(DataIndexes[v + 2], ExistPosition, ExistTexcoord, ExistNormal);

				// Position
				PushVertexData(std::get<0>(DataStructure_0), 3, SrcPositions, ResultPositions);
				PushVertexData(std::get<0>(DataStructure_1), 3, SrcPositions, ResultPositions);
				PushVertexData(std::get<0>(DataStructure_2), 3, SrcPositions, ResultPositions);
				
				// Texcoord
				if (ExistTexcoord)
				{
					PushVertexData(std::get<1>(DataStructure_0), 2, SrcTexcoords, ResultTexcoords);
					PushVertexData(std::get<1>(DataStructure_1), 2, SrcTexcoords, ResultTexcoords);
					PushVertexData(std::get<1>(DataStructure_2), 2, SrcTexcoords, ResultTexcoords);
				}

				// Normal
				if (ExistNormal)
				{
					PushVertexData(std::get<2>(DataStructure_0), 3, SrcNormals, ResultNormals);
					PushVertexData(std::get<2>(DataStructure_1), 3, SrcNormals, ResultNormals);
					PushVertexData(std::get<2>(DataStructure_2), 3, SrcNormals, ResultNormals);
				}
				else
				{
					// 法線をSurfaceのポジションから再計算する
					glm::vec3 Normal = RecalcNormalFromLastPosition(SrcPositions, NumOfVertex,
						std::get<0>(DataStructure_0), std::get<0>(DataStructure_1), std::get<0>(DataStructure_2));

					// 3頂点全部に同じ法線だとフラットシェーディングになってしまうので何か補正を考える必要があるかも
					// 1つの頂点が３つのポリゴンで共有しているとするとその３つのポリゴンにおける法線を全部足して最後に正規化すれば
					// スムーズシェーディング用の法線が計算できる
					/*
					  ______
					 /\    /\
					/  \  /  \
				   /____\/____\
					*/

					AddVertexNormal(NumOfVertex, std::get<0>(DataStructure_0), 3, Normal, ResultNormals);
					AddVertexNormal(NumOfVertex, std::get<0>(DataStructure_1), 3, Normal, ResultNormals);
					AddVertexNormal(NumOfVertex, std::get<0>(DataStructure_2), 3, Normal, ResultNormals);
				}

				// DataIndexesはポリゴン構成順に並んでいるので頂点インデックスはそのままインデックスをインクリメントしていくだけでいい
				int VertexIndex = static_cast<int>(UINTIndices.size());
				UINTIndices.push_back(VertexIndex + 0);
				UINTIndices.push_back(VertexIndex + 1);
				UINTIndices.push_back(VertexIndex + 2);
			}

			// 法線を正規化
			if (!ExistNormal)
			{
				NormalizeNormals(ResultNormals);
			}

			// アトリビュートと紐づけ
			{
				{
					ReservedVertexDataList.emplace("POSITION", ResultPositions);
					ReservedDataTypeList.emplace("POSITION", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("POSITION", 0);
				}

				{
					ReservedVertexDataList.emplace("NORMAL", ResultNormals);
					ReservedDataTypeList.emplace("NORMAL", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("NORMAL", 0);
				}

				{
					ReservedVertexDataList.emplace("TEXCOORD_0", ResultTexcoords);
					ReservedDataTypeList.emplace("TEXCOORD_0", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TEXCOORD_0", 0);
				}

				{
					// 接線は後で再計算するので今は0で初期化しておく
					std::vector<float> AttributeData;
					AttributeData.resize(NumOfVertex * 4, 0.0f);

					ReservedVertexDataList.emplace("TANGENT", AttributeData);
					ReservedDataTypeList.emplace("TANGENT", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TANGENT", 0);
				}

				{
					std::vector<float> AttributeData;
					AttributeData.resize(NumOfVertex * 4, 0.0f);

					ReservedVertexDataList.emplace("JOINTS_0", AttributeData);
					ReservedDataTypeList.emplace("JOINTS_0", graphics::EDataType::TYPE_UNSIGNED_INT);
					ReservedByteStrideList.emplace("JOINTS_0", 4 * 4);
				}

				{
					std::vector<float> AttributeData;
					AttributeData.resize(NumOfVertex * 4, 0.0f);

					ReservedVertexDataList.emplace("WEIGHTS_0", AttributeData);
					ReservedDataTypeList.emplace("WEIGHTS_0", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("WEIGHTS_0", 0);
				}
			}

			// OBJは接線を持っていないので毎回再計算
			if (NeedRecalculateTangent)
			{
				if (Indices.size() > 0)
				{
					if (!RecalculateTangent(ReservedVertexDataList["TANGENT"], ReservedVertexDataList["POSITION"], ReservedVertexDataList["TEXCOORD_0"], Indices)) return false;
				}
				else if (UINTIndices.size() > 0)
				{
					if (!RecalculateTangentWithUINT(ReservedVertexDataList["TANGENT"], ReservedVertexDataList["POSITION"], ReservedVertexDataList["TEXCOORD_0"], UINTIndices)) return false;
				}
			}

			// 頂点バッファを構築
			auto VertexBuffer = pGraphicsAPI->CreateVertexBuffer();
			{
				for (const auto& AttribName : NeedAttribNameList)
				{
					// ディメンションを登録
					int Dimention = 1;

					if (AttribName == "POSITION" || AttribName == "NORMAL")
					{
						Dimention = 3;
					}
					else if (AttribName == "TEXCOORD_0")
					{
						Dimention = 2;
					}
					else if (AttribName == "TANGENT" || AttribName == "JOINTS_0" || AttribName == "WEIGHTS_0")
					{
						Dimention = 4;
					}

					DimentionList.push_back(Dimention);

					// 頂点バッファにデータを渡す
					VertexDataList.push_back(ReservedVertexDataList[AttribName]);

					// データタイプ
					DataTypeList.push_back(ReservedDataTypeList[AttribName]);

					// ByteStride
					ByteStrideList.push_back(ReservedByteStrideList[AttribName]);
				}

				// メッシュ情報を渡す
				VertexBuffer->SetVertices(VertexDataList);
				VertexBuffer->SetAttributeDimensions(DimentionList);
				VertexBuffer->SetAttribDataTypes(DataTypeList);
				VertexBuffer->SetAttribByteStrides(ByteStrideList);

				Mesh->AddVertexBuffer(VertexBuffer);
			}

			// インデックスバッファ生成
			auto IndexBuffer = pGraphicsAPI->CreateIndexBuffer();
			{
				if (Indices.size() > 0)
				{
					unsigned int size = math::CMath::CalcClosestPowerOfFour(static_cast<unsigned int>(Indices.size()));

					Indices.resize(size, 0);

					// Indicesを登録
					IndexBuffer->SetIndices(Indices);
				}
				else if (UINTIndices.size() > 0)
				{
					unsigned int size = math::CMath::CalcClosestPowerOfFour(static_cast<unsigned int>(UINTIndices.size()));

					UINTIndices.resize(size, 0);

					// Indicesを登録
					IndexBuffer->SetUINTIndices(UINTIndices);
				}

				Mesh->AddIndexBuffer(IndexBuffer);
			}

			// マテリアル生成
			
			// プリミティブを作成する
			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer);
			// AddMaterial

			Mesh->AddPrimitive(Primitive);
			Object->AddMesh(Mesh);
		}

		return true;
	}

	std::tuple<int, int, int> COBJImporter::GetDataStructure(const std::string& Src, bool ExistPosition, bool ExistTexcoord, bool ExistNormal)
	{
		std::tuple<int, int, int> DataStructure = std::make_tuple(-1, -1, -1);

		std::vector<std::string> ParamList = format::CStringFormatter::Sprit(Src, '/');
		std::reverse(ParamList.begin(), ParamList.end());

		for (int a = 0; a < static_cast<int>(EOBJAttributeType::Max); a++)
		{
			if (ParamList.empty()) break;

			EOBJAttributeType type = static_cast<EOBJAttributeType>(a);

			if (type == EOBJAttributeType::Position && ExistPosition)
			{
				int index = std::stoi(ParamList.back());

				std::get<0>(DataStructure) = index;

				ParamList.pop_back();
				ParamList.shrink_to_fit();
			}
			else if (type == EOBJAttributeType::Texcoord && ExistTexcoord)
			{
				int index = std::stoi(ParamList.back());

				std::get<1>(DataStructure) = index;

				ParamList.pop_back();
				ParamList.shrink_to_fit();
			}
			else if (type == EOBJAttributeType::Normal && ExistNormal)
			{
				int index = std::stoi(ParamList.back());

				std::get<2>(DataStructure) = index;

				ParamList.pop_back();
				ParamList.shrink_to_fit();
			}
		}

		return DataStructure;
	}

	void COBJImporter::PushVertexData(int Index, int Dimention, const std::vector<float>& SrcData, std::vector<float>& DstVertexData)
	{
		const int NumOfData = static_cast<int>(SrcData.size()) / Dimention;

		if (Index < 0 || Index >= NumOfData) return;

		for (int n = 0; n < Dimention; n++)
		{
			DstVertexData.push_back(SrcData[Index * Dimention + n]);
		}
	}

	glm::vec3 COBJImporter::RecalcNormalFromLastPosition(const std::vector<float>& SrcPositions, int NumOfData,
		int PosIndex_0, int PosIndex_1, int PosIndex_2)
	{
		// Wavefront OBJの頂点はPolygon Faceの順番で並んでいる想定
		glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);

		if (
			(PosIndex_0 >= 0 && PosIndex_0 < NumOfData) &&
			(PosIndex_1 >= 0 && PosIndex_1 < NumOfData) &&
			(PosIndex_2 >= 0 && PosIndex_2 < NumOfData)
		)
		{
			glm::vec3 Pos_0 = glm::vec3(
				SrcPositions[PosIndex_0 * 3 + 0],
				SrcPositions[PosIndex_0 * 3 + 1],
				SrcPositions[PosIndex_0 * 3 + 2]
			);

			glm::vec3 Pos_1 = glm::vec3(
				SrcPositions[PosIndex_1 * 3 + 0],
				SrcPositions[PosIndex_1 * 3 + 1],
				SrcPositions[PosIndex_1 * 3 + 2]
			);

			glm::vec3 Pos_2 = glm::vec3(
				SrcPositions[PosIndex_2 * 3 + 0],
				SrcPositions[PosIndex_2 * 3 + 1],
				SrcPositions[PosIndex_2 * 3 + 2]
			);

			glm::vec3 v01 = glm::normalize(Pos_1 - Pos_0);
			glm::vec3 v02 = glm::normalize(Pos_2 - Pos_0);

			Normal = glm::normalize(glm::cross(v01, v02));
		}

		return Normal;
	}

	void COBJImporter::AddVertexNormal(int NumOfData, int Index, int Dimention, const glm::vec3& Normal, std::vector<float>& ResultNormals)
	{
		if (Index < 0 || Index >= NumOfData) return;

		for (int n = 0; n < Dimention; n++)
		{
			ResultNormals[Index * Dimention + n] += Normal[n];
		}
	}

	void COBJImporter::NormalizeNormals(std::vector<float>& ResultNormals)
	{
		for (int i = 0; i < static_cast<int>(ResultNormals.size()); i += 3)
		{
			glm::vec3 Normal = glm::vec3(ResultNormals[i + 0], ResultNormals[i + 1], ResultNormals[i + 2]);
			Normal = glm::normalize(Normal);

			ResultNormals[i + 0] = Normal.x;
			ResultNormals[i + 1] = Normal.y;
			ResultNormals[i + 2] = Normal.z;
		}
	}


	bool COBJImporter::RecalculateTangent(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices)
	{
		for (int i = 0; i < Indices.size(); i += 3)
		{
			// 頂点情報を取得
			unsigned short Index0 = Indices[i + 0], Index1 = Indices[i + 1], Index2 = Indices[i + 2];

			glm::vec3 Pos0 = glm::vec3(PosotionData[Index0 * 3 + 0], PosotionData[Index0 * 3 + 1], PosotionData[Index0 * 3 + 2]);
			glm::vec3 Pos1 = glm::vec3(PosotionData[Index1 * 3 + 0], PosotionData[Index1 * 3 + 1], PosotionData[Index1 * 3 + 2]);
			glm::vec3 Pos2 = glm::vec3(PosotionData[Index2 * 3 + 0], PosotionData[Index2 * 3 + 1], PosotionData[Index2 * 3 + 2]);

			glm::vec2 Texcoord0 = glm::vec2(TexcoordData[Index0 * 2 + 0], TexcoordData[Index0 * 2 + 1]);
			glm::vec2 Texcoord1 = glm::vec2(TexcoordData[Index1 * 2 + 0], TexcoordData[Index1 * 2 + 1]);
			glm::vec2 Texcoord2 = glm::vec2(TexcoordData[Index2 * 2 + 0], TexcoordData[Index2 * 2 + 1]);

			// 計算に使用するデータの下準備
			glm::vec3 E1 = Pos0 - Pos1;
			glm::vec3 E2 = Pos2 - Pos1;
			glm::vec2 dUV1 = Texcoord0 - Texcoord1;
			glm::vec2 dUV2 = Texcoord2 - Texcoord1;

			float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

			glm::vec4 Tangent = glm::vec4(0.0f);
			glm::vec4 BioTangent = glm::vec4(0.0f);

			// 接線と複接線を計算
			Tangent.x = f * (dUV2.y * E1.x - dUV1.y * E2.x);
			Tangent.y = f * (dUV2.y * E1.y - dUV1.y * E2.y);
			Tangent.z = f * (dUV2.y * E1.z - dUV1.y * E2.z);

			BioTangent.x = f * (-dUV2.x * E1.x + dUV1.x * E2.x);
			BioTangent.y = f * (-dUV2.x * E1.y + dUV1.x * E2.y);
			BioTangent.z = f * (-dUV2.x * E1.z + dUV1.x * E2.z);

			// データを書き込む
			TangentData[Index0 * 4 + 0] = Tangent.x; TangentData[Index0 * 4 + 1] = Tangent.y; TangentData[Index0 * 4 + 2] = Tangent.z; TangentData[Index0 * 4 + 3] = Tangent.w;
			TangentData[Index1 * 4 + 0] = Tangent.x; TangentData[Index1 * 4 + 1] = Tangent.y; TangentData[Index1 * 4 + 2] = Tangent.z; TangentData[Index1 * 4 + 3] = Tangent.w;
			TangentData[Index2 * 4 + 0] = Tangent.x; TangentData[Index2 * 4 + 1] = Tangent.y; TangentData[Index2 * 4 + 2] = Tangent.z; TangentData[Index2 * 4 + 3] = Tangent.w;

			// BioTangentはShaderで計算する
			/*BioTangentData[Index0 * 4 + 0] = BioTangent.x; BioTangentData[Index0 * 4 + 1] = BioTangent.y; BioTangentData[Index0 * 4 + 2] = BioTangent.z; BioTangentData[Index0 * 4 + 3] = BioTangent.w;
			BioTangentData[Index1 * 4 + 0] = BioTangent.x; BioTangentData[Index1 * 4 + 1] = BioTangent.y; BioTangentData[Index1 * 4 + 2] = BioTangent.z; BioTangentData[Index1 * 4 + 3] = BioTangent.w;
			BioTangentData[Index2 * 4 + 0] = BioTangent.x; BioTangentData[Index2 * 4 + 1] = BioTangent.y; BioTangentData[Index2 * 4 + 2] = BioTangent.z; BioTangentData[Index2 * 4 + 3] = BioTangent.w;*/
		}

		return true;
	}

	bool COBJImporter::RecalculateTangentWithUINT(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned int>& Indices)
	{
		for (int i = 0; i < Indices.size(); i += 3)
		{
			// 頂点情報を取得
			unsigned int Index0 = Indices[i + 0], Index1 = Indices[i + 1], Index2 = Indices[i + 2];

			glm::vec3 Pos0 = glm::vec3(PosotionData[Index0 * 3 + 0], PosotionData[Index0 * 3 + 1], PosotionData[Index0 * 3 + 2]);
			glm::vec3 Pos1 = glm::vec3(PosotionData[Index1 * 3 + 0], PosotionData[Index1 * 3 + 1], PosotionData[Index1 * 3 + 2]);
			glm::vec3 Pos2 = glm::vec3(PosotionData[Index2 * 3 + 0], PosotionData[Index2 * 3 + 1], PosotionData[Index2 * 3 + 2]);

			glm::vec2 Texcoord0 = glm::vec2(TexcoordData[Index0 * 2 + 0], TexcoordData[Index0 * 2 + 1]);
			glm::vec2 Texcoord1 = glm::vec2(TexcoordData[Index1 * 2 + 0], TexcoordData[Index1 * 2 + 1]);
			glm::vec2 Texcoord2 = glm::vec2(TexcoordData[Index2 * 2 + 0], TexcoordData[Index2 * 2 + 1]);

			// 計算に使用するデータの下準備
			glm::vec3 E1 = Pos0 - Pos1;
			glm::vec3 E2 = Pos2 - Pos1;
			glm::vec2 dUV1 = Texcoord0 - Texcoord1;
			glm::vec2 dUV2 = Texcoord2 - Texcoord1;

			float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

			glm::vec4 Tangent = glm::vec4(0.0f);
			glm::vec4 BioTangent = glm::vec4(0.0f);

			// 接線と複接線を計算
			Tangent.x = f * (dUV2.y * E1.x - dUV1.y * E2.x);
			Tangent.y = f * (dUV2.y * E1.y - dUV1.y * E2.y);
			Tangent.z = f * (dUV2.y * E1.z - dUV1.y * E2.z);

			BioTangent.x = f * (-dUV2.x * E1.x + dUV1.x * E2.x);
			BioTangent.y = f * (-dUV2.x * E1.y + dUV1.x * E2.y);
			BioTangent.z = f * (-dUV2.x * E1.z + dUV1.x * E2.z);

			// データを書き込む
			TangentData[Index0 * 4 + 0] = Tangent.x; TangentData[Index0 * 4 + 1] = Tangent.y; TangentData[Index0 * 4 + 2] = Tangent.z; TangentData[Index0 * 4 + 3] = Tangent.w;
			TangentData[Index1 * 4 + 0] = Tangent.x; TangentData[Index1 * 4 + 1] = Tangent.y; TangentData[Index1 * 4 + 2] = Tangent.z; TangentData[Index1 * 4 + 3] = Tangent.w;
			TangentData[Index2 * 4 + 0] = Tangent.x; TangentData[Index2 * 4 + 1] = Tangent.y; TangentData[Index2 * 4 + 2] = Tangent.z; TangentData[Index2 * 4 + 3] = Tangent.w;

			// BioTangentはShaderで計算する
			/*BioTangentData[Index0 * 4 + 0] = BioTangent.x; BioTangentData[Index0 * 4 + 1] = BioTangent.y; BioTangentData[Index0 * 4 + 2] = BioTangent.z; BioTangentData[Index0 * 4 + 3] = BioTangent.w;
			BioTangentData[Index1 * 4 + 0] = BioTangent.x; BioTangentData[Index1 * 4 + 1] = BioTangent.y; BioTangentData[Index1 * 4 + 2] = BioTangent.z; BioTangentData[Index1 * 4 + 3] = BioTangent.w;
			BioTangentData[Index2 * 4 + 0] = BioTangent.x; BioTangentData[Index2 * 4 + 1] = BioTangent.y; BioTangentData[Index2 * 4 + 2] = BioTangent.z; BioTangentData[Index2 * 4 + 3] = BioTangent.w;*/
		}

		return true;
	}

}