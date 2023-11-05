#ifdef USE_FBX
#include "CFBXImporter.h"

#include "../Object/C3DObject.h"

#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkin.h"
#include "../Animation/CJoint.h"

namespace fbx
{
	bool CFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> DummyAnimationClipList;

		if (!Import(pGraphicsAPI, Data, true, Object, DummyAnimationClipList, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		return true;
	}

	bool CFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		std::shared_ptr<object::C3DObject> DummyObject = std::make_shared<object::C3DObject>("", "");

		if (!Import(pGraphicsAPI, Data, false, DummyObject, AnimationClipList, nullptr, nullptr, nullptr, nullptr)) return false;

		return true;
	}

	bool CFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		// 全体のメモリやObjectを管理するManagerを作成
		fbxsdk::FbxManager* Manager = fbxsdk::FbxManager::Create();

		// IO Object(Input/Output)を作成
		fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(Manager, IOSROOT);
		Manager->SetIOSettings(ios);

		// Importerを生成
		fbxsdk::FbxImporter* Importer = fbxsdk::FbxImporter::Create(Manager, "");

		// ひとまず適当にファイル名からImport(あとでfromMemoryにする)
		//const char* fileName = "Resources\\Motions\\Walking.fbx";
		const char* fileName = "Resources\\Motions\\Walking_WithSkin.fbx";

		// Importerを初期化
		if (!Importer->Initialize(fileName, -1, Manager->GetIOSettings()))
		{
			Console::Log("Failed to Import FBX\n");

			return false;
		}

		// シーンを作成
		// Sceneの中にMesh・ライト・スケルトンなどが入っている.
		// Blenderの画面と同じようなものかな
		fbxsdk::FbxScene* Scene = fbxsdk::FbxScene::Create(Manager, "myScene");

		// Importを実行
		Importer->Import(Scene);

		// FBXの解析開始
		if (!Analyse(pGraphicsAPI, Scene, IsUseObject, Object, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		// FBX解析を終了
		Manager->Destroy();

		return true;
	}

	bool CFBXImporter::Analyse(api::IGraphicsAPI* pGraphicsAPI, fbxsdk::FbxScene* Scene, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		// 描画情報の取得
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		std::vector<std::shared_ptr<animation::CSkin>> AnimationSkinList;

		std::vector<FbxMesh*> pFbxMeshList;
		
		if (IsUseObject)
		{
			fbxsdk::FbxNode* RootNode = Scene->GetRootNode();
			if (RootNode)
			{
				if (!Analyse(pGraphicsAPI, pFbxMeshList, createInfo, RootNode, TextureList, MaterialList, MeshList)) return false;
			}
		}

		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;

		// 重複を除く
		std::unique(pFbxMeshList.begin(), pFbxMeshList.end());

		if (!CreateNodeList(Scene, pFbxMeshList, NodeList, MeshList, MaterialList, RootNodeIndexList)) return false;

		// アニメーション
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		// オブジェクトを生成
		if (IsUseObject)
		{
			// オブジェクトにリソースを登録
			for (const auto& Material : MaterialList)
			{
				Object->AddMaterial(Material);
			}

			for (const auto& Mesh : MeshList)
			{
				Object->AddMesh(Mesh);
			}

			for (const auto& Node : NodeList)
			{
				Object->AddNode(Node);
			}

			for (const auto& Skin : AnimationSkinList)
			{
				Object->AddAnimationSkin(Skin);
			}

			for (const auto& Clip : AnimationClipList)
			{
				Object->AddAnimationClip(Clip);
			}

			Object->SetRootNodeIndexList(RootNodeIndexList);

			// オブジェクトを生成
			if (!Object->Create(pGraphicsAPI, DepthVertex, DepthFragment, TextureSet)) return false;
		}

		return true;
	}

	bool CFBXImporter::Analyse(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo,
		fbxsdk::FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
		std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		// テクスチャ

		// マテリアル

		// メッシュ
		if (!CreateMesh(pFBXNode, pFbxMeshList, MeshList)) return false;

		// マテリアルを持っていないのならダミーを渡す
		if (MaterialList.size() <= 0)
		{
			if (!CreateDummyMaterial(pGraphicsAPI, pFBXNode, MaterialList, createInfo, MeshList)) return false;
		}

		// スキン

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!Analyse(pGraphicsAPI, pFbxMeshList, createInfo, pFBXNode->GetChild(i), TextureList, MaterialList, MeshList)) return false;
		}

		return true;
	}

	bool CFBXImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, fbxsdk::FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		// マテリアルにシェーダーを設定
		std::shared_ptr<graphics::CMaterial> material = pGraphicsAPI->CreateMaterial(createInfo);

		// UBO
		{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });

			// UBOの初期値を設定する
			UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
			UniformBuffer->AddData("lightDir", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
			UniformBuffer->AddData("lightColor", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
			UniformBuffer->AddData("cameraPos", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
			UniformBuffer->AddData("baseColorFactor", &glm::vec4(1.0f)[0], sizeof(float) * 4, 0);
			UniformBuffer->AddData("emissiveFactor", &glm::vec4(0.0f)[0], sizeof(float) * 4, 0);
			UniformBuffer->AddData("time", &glm::vec1(0.0f)[0], sizeof(float), 0);
			UniformBuffer->AddData("metallicFactor", &glm::vec1(0.5f)[0], sizeof(float), 0);
			UniformBuffer->AddData("roughnessFactor", &glm::vec1(0.5f)[0], sizeof(float), 0);
			UniformBuffer->AddData("normalMapScale", &glm::vec1(1.0f)[0], sizeof(float), 0);
			UniformBuffer->AddData("occlusionStrength", &glm::vec1(1.0f)[0], sizeof(float), 0);

			float MipCount = 1.0f;
			UniformBuffer->AddData("mipCount", &glm::vec1(MipCount)[0], sizeof(float), 0);

			int ShadowMapX = 1, ShadowMapY = 1;
			UniformBuffer->AddData("ShadowMapX", &glm::vec1(static_cast<float>(ShadowMapX))[0], sizeof(float), 0);
			UniformBuffer->AddData("ShadowMapY", &glm::vec1(static_cast<float>(ShadowMapY))[0], sizeof(float), 0);

			// テクスチャを紐づける
			material->AddTextureBindingLayout({ "baseColorTexture", 2, 3, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useBaseColorTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "metallicRoughnessTexture", 4, 5, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useMetallicRoughnessTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "emissiveTexture", 6, 7, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useEmissiveTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "normalTexture", 8, 9, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useNormalTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "occlusionTexture", 10, 11, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useOcclusionTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "cubemapTexture", 12, 13, -1, graphics::ETextureUsage::TEXTURE_USAGE_CUBE });
			UniformBuffer->AddData("useCubeMap", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "shadowmapTexture", 14, 15, -1, graphics::ETextureUsage::TEXTURE_USAGE_FRAME });
			UniformBuffer->AddData("useShadowMap", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "IBL_Diffuse_Texture", 16, 17, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse });
			material->AddTextureBindingLayout({ "IBL_Specular_Texture", 18, 19, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular });
			material->AddTextureBindingLayout({ "IBL_GGXLUT_Texture", 20, 21, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT });
			UniformBuffer->AddData("useIBL", &glm::ivec1(0)[0], sizeof(int), 0);

			UniformBuffer->AddData("useSkinMeshAnimation", &glm::ivec1(0)[0], sizeof(int), 0);
			UniformBuffer->AddData("pad0", &glm::ivec1(0)[0], sizeof(int), 0);
			UniformBuffer->AddData("pad1", &glm::ivec1(0)[0], sizeof(int), 0);
			UniformBuffer->AddData("pad2", &glm::ivec1(0)[0], sizeof(int), 0);

			// マテリアルにUBOを割り当てる
			material->AddShaderBuffer(UniformBuffer);
		}

		// SkinMatrix StorageBuffer
		{
			auto SSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("SkinMatrixBuffer", 1, false) }, graphics::EBufferUpdateType::UPDATE_TYPE_CPU);

			int SkinMatCount = 0;
			//for (const auto& glTFSkin : model.skins) { SkinMatCount += static_cast<int>(glTFSkin.joints.size()); }

			if (SkinMatCount <= 0) SkinMatCount = 1;

			std::vector<glm::mat4> SkinMatrixList;
			SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

			SSBO->AddData("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);

			material->AddShaderBuffer(SSBO);
		}

		material->SetCullMode(graphics::ECullMode::CULL_NONE);

		MaterialList.push_back(material);

		for (auto& Mesh : MeshList)
		{
			for (auto& Primirive : Mesh->GetPrimitiveList())
			{
				Primirive->SetMaterialIndex(0);
			}
		}

		return true;
	}

	bool CFBXImporter::CreateMesh(fbxsdk::FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		fbxsdk::FbxMesh* pFbxMesh = pFBXNode->GetMesh();
		
		if (pFbxMesh)
		{
			pFbxMeshList.push_back(pFbxMesh);

			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			{
				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

				// 頂点バッファ本体
				std::vector<std::vector<float>> VertexDataList;
				std::vector<int> DimentionList;
				std::vector<renderer::EDataType> DataTypeList;
				std::vector<int> ByteStrideList;

				std::vector<unsigned short> Indices;

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

				// インデックスバッファを読む
				{
					for (int i = 0; i < pFbxMesh->GetPolygonCount(); i++)
					{
						// 1ポリゴン内の頂点数を取得
						int lPolygonSize = pFbxMesh->GetPolygonSize(i);

						if (lPolygonSize == 3)
						{
							for (int j = 0; j < 3; j++)
							{
								// インデックスバッファを読む
								int IndexVal = pFbxMesh->GetPolygonVertex(i, j);
								Indices.push_back(IndexVal);
							}
						}
						else if (lPolygonSize == 4)
						{
							// 四角形ポリゴンを三角ポリゴンに変換する際に使用するインデックス
							int IndexArray[6] = { 0, 1, 2, 0, 2, 3 };

							for (int j = 0; j < 6; j++)
							{
								// インデックスバッファを読む
								int IndexVal = pFbxMesh->GetPolygonVertex(i, IndexArray[j]);
								Indices.push_back(IndexVal);
							}
						}
					}
				}

				// 頂点数
				int VertexCount = pFbxMesh->GetPolygonVertexCount();

				// 頂点バッファを読む
				{
					// 頂点座標
					{
						std::vector<float> AttributeData;

						for (int i = 0; i < pFbxMesh->GetControlPointsCount(); i++)
						{
							fbxsdk::FbxVector4 pPosition = pFbxMesh->GetControlPointAt(i);

							// 頂点座標リストから頂点を取得する
							for (int j = 0; j < 3; j++)
							{
								AttributeData.push_back(static_cast<float>(pPosition[j]));
							}
						}

						// データを登録
						ReservedVertexDataList.insert({ "POSITION" ,AttributeData });

						// コンポーネントタイプ(データ型)を取得
						renderer::EDataType attribComponentType = renderer::EDataType::TYPE_FLOAT;
						ReservedDataTypeList.insert({ "POSITION", attribComponentType });

						// ByteStrideを取得
						// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
						// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
						int attibByteStride = 0;
						ReservedByteStrideList.insert({ "POSITION", attibByteStride });
					}

					// 法線
					FbxArray<FbxVector4> pNormals;
					if (pFbxMesh->GetPolygonVertexNormals(pNormals))
					{
						int Len = static_cast<int>(pNormals.GetCount());

						// Bufferを取得
						std::vector<double> DoubleData(Len * 4);
						std::memcpy(&DoubleData[0], &pNormals[0][0], sizeof(pNormals[0][0]) * Len * 4);

						// Vector3のfloat Arrayに変換
						std::vector<float> AttributeData;
						for (int p = 0; p < DoubleData.size(); p++) { if ((p + 1) % 4 != 0) AttributeData.push_back(static_cast<float>(DoubleData[p])); }

						// データを登録
						ReservedVertexDataList.insert({ "NORMAL" ,AttributeData });

						// コンポーネントタイプ(データ型)を取得
						renderer::EDataType attribComponentType = renderer::EDataType::TYPE_FLOAT;
						ReservedDataTypeList.insert({ "NORMAL", attribComponentType });

						// ByteStrideを取得
						// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
						// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
						int attibByteStride = 0;
						ReservedByteStrideList.insert({ "NORMAL", attibByteStride });
					}

					// UV
					FbxArray<FbxVector2> pUVs;
					if (pFbxMesh->GetPolygonVertexUVs("", pUVs))
					{
						int Len = static_cast<int>(pUVs.GetCount());

						// Bufferを取得
						std::vector<double> DoubleData(Len * 2);
						std::memcpy(&DoubleData[0], &pUVs[0][0], sizeof(pUVs[0][0]) * Len * 2);

						// Vector3のfloat Arrayに変換
						std::vector<float> AttributeData(Len * 2);
						std::transform(DoubleData.begin(), DoubleData.end(), AttributeData.begin(), [](double val) {return static_cast<float>(val); });

						// データを登録
						ReservedVertexDataList.insert({ "TEXCOORD_0" ,AttributeData });

						// コンポーネントタイプ(データ型)を取得
						renderer::EDataType attribComponentType = renderer::EDataType::TYPE_FLOAT;
						ReservedDataTypeList.insert({ "TEXCOORD_0", attribComponentType });

						// ByteStrideを取得
						// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
						// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
						int attibByteStride = 0;
						ReservedByteStrideList.insert({ "TEXCOORD_0", attibByteStride });
					}

					// アトリビュートがまだ登録されていなければここで0埋めの値を渡す
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

						// アトリビュートがまだ登録されていなければここで0埋めの値を渡す
						if (ReservedVertexDataList.find(AttribName) == ReservedVertexDataList.end())
						{
							ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexCount * Dimention, 0.0f) });

							// 接線もしく複接線の再計算が必要
							if (AttribName == "TANGENT")
							{
								NeedRecalculateTangent = true;
							}

							// DataTypeとByteStrideの初期値をセット
							renderer::EDataType DataType = renderer::EDataType::TYPE_FLOAT;

							// 『JOINTS_0』はunsigned shortである
							if (AttribName == "JOINTS_0") DataType = renderer::EDataType::TYPE_UNSIGNED_SHORT;

							ReservedDataTypeList.insert({ AttribName, DataType });
							ReservedByteStrideList.insert({ AttribName, 0 });
						}
					}
				}

				// 頂点バッファを構築
				{
					for (const auto& AttribName : NeedAttribNameList)
					{
						// 頂点バッファにデータを渡す
						VertexDataList.push_back(ReservedVertexDataList[AttribName]);

						// データタイプ
						DataTypeList.push_back(ReservedDataTypeList[AttribName]);

						// ByteStride
						ByteStrideList.push_back(ReservedByteStrideList[AttribName]);

					}
				}

				// メッシュ情報を渡す
				createInfo->SetVertices(VertexDataList);
				createInfo->SetAttributeDimensions(DimentionList);
				createInfo->SetAttribDataTypes(DataTypeList);
				createInfo->SetAttribByteStrides(ByteStrideList);

				// Indicesを登録
				createInfo->SetIndices(Indices);

				// プリミティブを作成する
				int MaterialIndex = 0; // ひとまず0番目のダミーマテリアルを渡しておく
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
				Mesh->AddPrimitive(Primitive);
			}

			MeshList.push_back(Mesh);
		}

		return true;
	}

	bool CFBXImporter::CreateNodeList(fbxsdk::FbxScene* Scene, const std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		std::vector<fbxsdk::FbxNode*> pFbxNodeList;

		// ルートノードを取得
		fbxsdk::FbxNode* RootNode = Scene->GetRootNode();
		if (RootNode)
		{
			RootNodeIndexList.push_back(std::vector<int>(0));

			if (!CreateNode(RootNode, pFbxMeshList, pFbxNodeList, NodeList, MeshList, MaterialList)) return false;
		}

		// 子要素を登録する
		if (pFbxNodeList.size() != NodeList.size()) return false;

		for (int n = 0; n < NodeList.size(); n++)
		{
			fbxsdk::FbxNode* pFBXNode = pFbxNodeList[n];
			const auto& Node = NodeList[n];
			
			std::vector<int> ChildNodeList;

			for (int i = 0; i < pFBXNode->GetChildCount(); i++)
			{
				fbxsdk::FbxNode* pChildNode = pFBXNode->GetChild(i);

				if (pChildNode)
				{
					for (int ChildNodeIndex = 0; ChildNodeIndex < pFbxNodeList.size(); ChildNodeIndex++)
					{
						if (pChildNode == pFbxNodeList[ChildNodeIndex])
						{
							ChildNodeList.push_back(ChildNodeIndex);

							break;
						}
					}
				}
			}

			Node->SetChildrenNodeIndexList(ChildNodeList);
		}

		return true;
	}

	bool CFBXImporter::CreateNode(fbxsdk::FbxNode* pFBXNode, const std::vector<FbxMesh*>& pFbxMeshList, std::vector<FbxNode*>& pFbxNodeList,
		std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList)
	{
		// Listの要素のポインターとpFBXNodeが持ってる要素のポインターを比較してリスト内の順番をIndexとして渡す
		int MeshIndex = -1;
		int Loop = 0;
		for (auto it = pFbxMeshList.begin(); it != pFbxMeshList.end(); it++)
		{
			fbxsdk::FbxMesh* pTargetMesh = *it;
			if (pTargetMesh && pTargetMesh == pFBXNode->GetMesh())
			{
				MeshIndex = Loop;

				break;
			}

			Loop++;
		}

		int SkinIndex = -1; // ひとまず無し

		// Nodeを作成
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, MeshList, MaterialList);

		std::string NodeName = std::string(pFBXNode->GetName());
		Node->SetName(NodeName);

		Node->SetSkinIndex(SkinIndex);

		fbxsdk::FbxDouble3 fbxTranslation = pFBXNode->LclTranslation.Get();
		fbxsdk::FbxDouble3 fbxRotation = pFBXNode->LclRotation.Get();
		fbxsdk::FbxDouble3 fbxScale = pFBXNode->LclScaling.Get();

		glm::vec3 Pos = glm::vec3(static_cast<float>(fbxTranslation[0]), static_cast<float>(fbxTranslation[1]), static_cast<float>(fbxTranslation[2]));
		glm::quat Rotation = 
			glm::angleAxis(static_cast<float>(fbxRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::angleAxis(static_cast<float>(fbxRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::angleAxis(static_cast<float>(fbxRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 Scale = glm::vec3(static_cast<float>(fbxScale[0]), static_cast<float>(fbxScale[1]), static_cast<float>(fbxScale[2]));

		Node->SetPos(Pos);
		Node->SetRot(Rotation);
		Node->SetScale(Scale);

		// ノードを登録
		NodeList.push_back(Node);

		pFbxNodeList.push_back(pFBXNode);

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateNode(pFBXNode->GetChild(i), pFbxMeshList, pFbxNodeList, NodeList, MeshList, MaterialList)) return false;
		}

		return true;
	}
}
#endif // USE_FBX