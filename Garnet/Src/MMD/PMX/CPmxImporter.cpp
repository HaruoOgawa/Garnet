#ifdef USE_MMD
#include "CPmxImporter.h"
#include "../../Debug/Message/Console.h"
#include "../../Math/CMath.h"
#include "../../Object/C3DObject.h"
#include "../../Animation/CAnimationClip.h"
#include "../../Animation/CSkin.h"
#include "../../Animation/CJoint.h"
#include "../../Animation/CBoneNameProvider.h"

#include "../../Graphics/CMaterialFrame.h"

namespace mmd
{
	bool CPmxImporter::ImportPmx(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		CPmxModel model;

		// バイナリの解析
		if (!model.Analyse(Data))
		{
			Console::Log("[Cpp Log] Error - Failed to Analyse Pmx Binary.\n");

			return false;
		}

		// ノードリスト
		// PMXにはNodeの概念が無さそうなのでいい感じに調整する
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;

		std::shared_ptr<object::CNode> RootNode = std::make_shared<object::CNode>(-1, std::vector<std::shared_ptr<graphics::CMesh>>(), std::vector<std::shared_ptr<graphics::CMaterial>>());
		NodeList.push_back(RootNode);

		RootNodeIndexList.push_back(std::vector<int>(0));

		// Skin
		std::shared_ptr<animation::CSkin> Skin = std::make_shared<animation::CSkin>();

		// マテリアルリスト
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;

		// マテリアルを持っていないのならダミーを渡す
		if (MaterialList.size() <= 0)
		{
			if (!CreateDummyMaterial(pGraphicsAPI, MaterialList, MaterialFrame, Skin)) return false;
		}

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMesh(model, MeshList, RootNode, NodeList, MaterialList)) return false;

		// リソースを登録
		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		Object->AddAnimationSkin(Skin);

		for (const auto& Material : MaterialList)
		{
			Object->AddMaterial(Material);
		}

		for (const auto& Mesh : MeshList)
		{
			Object->AddMesh(Mesh);
		}

		return true;
	}

	bool CPmxImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkin>& Skin)
	{
		if (!MaterialFrame) return false;

		// マテリアルにシェーダーを設定
		std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI);

		// SkinMatrix StorageBuffer
		{
			// SkinMatは存在するJointの数だけ用意する必要がある
			unsigned int SkinMatCount = 1;
			if (Skin && Skin->GetJointList().size() > 0) SkinMatCount = static_cast<unsigned int>(Skin->GetJointList().size());

			// SSBOのサイズは2のn乗である必要がある
			SkinMatCount = math::CMath::CalcNextPowerOfTwo(SkinMatCount);

			std::vector<glm::mat4> SkinMatrixList;
			SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

			material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
		}

		//material->SetCullMode(graphics::ECullMode::CULL_NONE);

		MaterialList.push_back(material);

		return true;
	}

	bool CPmxImporter::CreateMesh(const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<object::CNode>& RootNode, std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList)
	{
		{
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			{
				// 仮で0とする
				int MaterialIndex = 0;

				//
				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

				// 頂点バッファ本体
					std::vector<std::vector<float>> VertexDataList;
				std::vector<int> DimentionList;
				std::vector<renderer::EDataType> DataTypeList;
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
				std::map<std::string, renderer::EDataType> ReservedDataTypeList;
				std::map<std::string, int> ReservedByteStrideList;

				// タンジェントの計算が必要
				bool NeedRecalculateTangent = false;

				// メタデータ
				const auto& MetaData = model.GetMetaData();
				const auto& PmxMesh = model.GetPmxMesh();

				// 頂点バッファを読む
				{
					{
						ReservedVertexDataList.emplace("POSITION", PmxMesh->GetPositionAttribute());
						ReservedDataTypeList.emplace("POSITION", renderer::EDataType::TYPE_FLOAT);
						ReservedByteStrideList.emplace("POSITION", 4 * 3);
					}

					{
						ReservedVertexDataList.emplace("NORMAL", PmxMesh->GetNormalAttribute());
						ReservedDataTypeList.emplace("NORMAL", renderer::EDataType::TYPE_FLOAT);
						ReservedByteStrideList.emplace("NORMAL", 4 * 3);
					}

					{
						ReservedVertexDataList.emplace("TEXCOORD_0", PmxMesh->GetUVAttribute());
						ReservedDataTypeList.emplace("TEXCOORD_0", renderer::EDataType::TYPE_FLOAT);
						ReservedByteStrideList.emplace("TEXCOORD_0", 4 * 2);
					}

					{
						std::vector<float> AttributeData;
						AttributeData.resize(4 * (PmxMesh->GetPositionAttribute().size() / 3), 0.0f);

						ReservedVertexDataList.emplace("TANGENT", AttributeData);
						ReservedDataTypeList.emplace("TANGENT", renderer::EDataType::TYPE_FLOAT);
						ReservedByteStrideList.emplace("TANGENT", 4 * 2);
					}

					{
						std::vector<float> AttributeData;

						if (MetaData.BoneIndexSize == 1)
						{
							const auto& ByteJointAttribute = PmxMesh->GetByteJointAttribute();

							AttributeData.resize(ByteJointAttribute.size() / 4);
							std::memcpy(&AttributeData[0], &ByteJointAttribute[0], sizeof(unsigned char) * ByteJointAttribute.size());

							ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_BYTE);
							ReservedByteStrideList.emplace("JOINTS_0", 1 * 4);
						}
						else if (MetaData.BoneIndexSize == 2)
						{
							const auto& UShortJointAttribute = PmxMesh->GetUShortJointAttribute();

							AttributeData.resize(UShortJointAttribute.size() / 2);
							std::memcpy(&AttributeData[0], &UShortJointAttribute[0], sizeof(unsigned short) * UShortJointAttribute.size());

							ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_SHORT);
							ReservedByteStrideList.emplace("JOINTS_0", 2 * 4);
						}
						else if (MetaData.BoneIndexSize == 4)
						{
							const auto& IntJointAttribute = PmxMesh->GetUIntJointAttribute();

							AttributeData.resize(IntJointAttribute.size());
							std::memcpy(&AttributeData[0], &IntJointAttribute[0], sizeof(unsigned int) * IntJointAttribute.size());

							ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_INT);
							ReservedByteStrideList.emplace("JOINTS_0", 4 * 4);
						}

						ReservedVertexDataList.emplace("JOINTS_0", AttributeData);
						
					}

					{
						ReservedVertexDataList.emplace("WEIGHTS_0", PmxMesh->GetWeightAttribute());
						ReservedDataTypeList.emplace("WEIGHTS_0", renderer::EDataType::TYPE_FLOAT);
						ReservedByteStrideList.emplace("WEIGHTS_0", 4 * 4);
					}
				}

				// 頂点バッファを構築
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
					createInfo->SetVertices(VertexDataList);
					createInfo->SetAttributeDimensions(DimentionList);
					createInfo->SetAttribDataTypes(DataTypeList);
					createInfo->SetAttribByteStrides(ByteStrideList);
				}

				// インデックスバッフを読む
				{
					if (MetaData.VertexIndexSize == 1)
					{
						// 未対応
						Console::Log("[Error] InValid Indices Type - Byte\n");

						return false;
					}
					else if (MetaData.VertexIndexSize == 2)
					{
						// Indicesを登録
						createInfo->SetIndices(PmxMesh->GetUShortIndices());
					}
					else if (MetaData.VertexIndexSize == 4)
					{
						createInfo->SetUINTIndices(PmxMesh->GetUIntIndices());
					}
				}

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
				Mesh->AddPrimitive(Primitive);
			}

			MeshList.push_back(Mesh);

			// PMXにはノードの概念がないのでこちらで明示的に作成する
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(static_cast<int>(MeshList.size()) - 1, MeshList, MaterialList);

			NodeList.push_back(Node);

			// RootNodeに子要素を登録する
			std::vector<int> ChildrenNodeIndexList = RootNode->GetChildrenNodeIndexList();
			ChildrenNodeIndexList.push_back(static_cast<int>(NodeList.size()) - 1);

			RootNode->SetChildrenNodeIndexList(ChildrenNodeIndexList);
		}

		return true;
	}
}
#endif // USE_MMD