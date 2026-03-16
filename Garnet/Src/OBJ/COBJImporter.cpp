#include "COBJImporter.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include "../../Graphics/CMaterialFrame.h"
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
		if (!Build(Object, BaseMaterialFrameList, Positions, Texcoords, Normals, VertexDataIndexes)) return false;

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

	bool COBJImporter::Build(const std::shared_ptr<object::C3DObject>& Object,
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
			}

			// 法線を正規化
			if (!ExistNormal)
			{
				NormalizeNormals(ResultNormals);
			}

			// メッシュ・マテリアル生成
			
			//std::shared_ptr<graphics::CPrimitive>
			//Object->AddMesh
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
}