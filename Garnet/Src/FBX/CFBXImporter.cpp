#ifdef USE_FBX
#include "CFBXImporter.h"
#include "CFBXStream.h"
#include "CFBXMomoryStream.h"
#include "../Animation/CBoneNameProvider.h"

#include "../Object/C3DObject.h"

#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkin.h"
#include "../Animation/CJoint.h"

using namespace fbxsdk;

namespace fbx
{
	bool CFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		if (!Import(pGraphicsAPI, Data, true, Object, AnimationClipList, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		return true;
	}

	bool CFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

		if (!Import(pGraphicsAPI, Data, false, Object, AnimationClipList, nullptr, nullptr, nullptr, nullptr)) return false;

		return true;
	}

	bool CFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		// 全体のメモリやObjectを管理するManagerを作成
		FbxManager* Manager = FbxManager::Create();

		// IO Object(Input/Output)を作成
		FbxIOSettings* ios = FbxIOSettings::Create(Manager, IOSROOT);
		Manager->SetIOSettings(ios);

		// Importerを生成
		FbxImporter* Importer = FbxImporter::Create(Manager, "");

		// ひとまず適当にファイル名からImport(あとでfromMemoryにする)
		//const char* fileName = "Resources\\Motions\\Walking.fbx";
		const char* fileName = "Resources\\Motions\\Walking_WithSkin.fbx";
		//const char* fileName = "Resources\\Motions\\Locking Hip Hop Dance.fbx";

		// Streamを作成
		CFBXMomoryStream Stream(Manager, Data);
		void* streamData = NULL;

		// Importerを初期化
		//if (!Importer->Initialize(fileName, -1, Manager->GetIOSettings()))
		if (!Importer->Initialize(&Stream, streamData, -1, Manager->GetIOSettings()))
		{
			Console::Log("Failed to Import FBX\n");

			return false;
		}

		// シーンを作成
		// Sceneの中にMesh・ライト・スケルトンなどが入っている.
		// Blenderの画面と同じようなものかな
		FbxScene* Scene = FbxScene::Create(Manager, "myScene");

		// Importを実行
		Importer->Import(Scene);

		int UpDir;
		Scene->GetGlobalSettings().GetAxisSystem().GetUpVector(UpDir);

		int Coordinate = Scene->GetGlobalSettings().GetAxisSystem().GetCoorSystem();

		// FBXの解析開始
		if (!Analyse(pGraphicsAPI, Scene, IsUseObject, Object, AnimationClipList, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		// FBX解析を終了
		Manager->Destroy();

		return true;
	}

	bool CFBXImporter::Analyse(api::IGraphicsAPI* pGraphicsAPI, FbxScene* Scene, bool IsUseObject, std::shared_ptr<object::C3DObject>& Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		FbxNode* RootNode = Scene->GetRootNode();

		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;
		std::vector<FbxNode*> pFbxNodeList;

		if (!CreateNodeList(Scene, pFbxNodeList, NodeList, RootNodeIndexList)) return false;

		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		Object->SetRootNodeIndexList(RootNodeIndexList);

		// DefaultMatrixを保存
		Object->ApplyDefaultLocalTransform();

		// WorldMatrixを計算
		Object->CalcWorldMatrix();

		// ParentNodeを設定する
		Object->ApplyParentNode();

		// Skin
		std::shared_ptr<animation::CSkin> Skin = std::make_shared<animation::CSkin>();
		std::vector<FbxNode*> FbxJointList;
		if (RootNode)
		{
			if (!CreateAnimationSkin(RootNode, Skin, FbxJointList, NodeList)) return false;
		}

		Object->AddAnimationSkin(Skin);

		// BoneTableを作成
		Skin->MakeBoneTable();

		// DefaultLocalTransformを保存する
		Object->ApplyDefaultLocalTransform();

		// 親ノードを設定
		Object->ApplyParentNode();

		// ワールド行列の計算
		Object->CalcWorldMatrix();

		// 親のJointを追加
		ApplyParentJointList(Skin, NodeList);

		// アニメーション
		if (!CreateAnimation(Scene, AnimationClipList, NodeList, Skin, FbxJointList)) return false;

		if (IsUseObject)
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
			std::vector<std::shared_ptr<graphics::CMesh>> MeshList;

			if (RootNode)
			{
				// 描画情報の取得
				std::vector<FbxMesh*> pFbxMeshList;
				if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, createInfo, RootNode, TextureList, MaterialList, MeshList, Skin)) return false;

				// マテリアルを持っていないのならダミーを渡す
				if (MaterialList.size() <= 0)
				{
					if (!CreateDummyMaterial(pGraphicsAPI, RootNode, MaterialList, createInfo, MeshList, Skin)) return false;
				}

				// 重複を除く
				for (;;)
				{
					auto result = std::unique(pFbxMeshList.begin(), pFbxMeshList.end());

					if (result != pFbxMeshList.end())
					{
						pFbxMeshList.erase(result);
					}
					else
					{
						break;
					}
				}

				pFbxMeshList.shrink_to_fit();

				// Nodeと各要素をIndexで繋ぐ
				if (!ConnectNodeTo(NodeList, pFbxNodeList, pFbxMeshList, MeshList, MaterialList, Skin)) return false;
			}

			// オブジェクトにリソースを登録
			for (const auto& Material : MaterialList)
			{
				Object->AddMaterial(Material);
			}

			for (const auto& Mesh : MeshList)
			{
				Object->AddMesh(Mesh);
			}

			// FrameCountが多いものと少ないものといった感じでFBXのAnimationClipは複数個存在することがある
			// どちらか一方がループ用の短いアニメーションだったり長いダンスモーションだったりするので
			// どちらを使用するかはユーザーがモーションを見てユーザーが選ぶようにする
			// 大抵は2つ目のクリップがどのモーションでも一番良いみたいだが、これがFBXの仕様なのかMixamoの仕様なのかわからないのでひとまずそういうことにしておく
			for (const auto& Clip : AnimationClipList)
			{
				Object->AddAnimationClip(Clip);
			}

			// オブジェクトを生成
			if (!Object->Create(pGraphicsAPI, DepthVertex, DepthFragment, TextureSet)) return false;
		}

		return true;
	}

	bool CFBXImporter::CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo,
		FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
		std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		// 知りたいのは描画情報なのでここではeMeshのみ見る
		if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			// テクスチャ

			// マテリアル

			// メッシュ
			if (!CreateMesh(pFBXNode, pFbxMeshList, MeshList, Skin)) return false;
		}
		

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, createInfo, pFBXNode->GetChild(i), TextureList, MaterialList, MeshList, Skin)) return false;
		}

		return true;
	}

	bool CFBXImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
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
			UniformBuffer->AddData("JointIndexOffset", &glm::ivec1(0)[0], sizeof(int), 0);
			UniformBuffer->AddData("pad1", &glm::ivec1(0)[0], sizeof(int), 0);
			UniformBuffer->AddData("pad2", &glm::ivec1(0)[0], sizeof(int), 0);

			// マテリアルにUBOを割り当てる
			material->AddShaderBuffer(UniformBuffer);
		}

		// SkinMatrix StorageBuffer
		{
			auto SSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("SkinMatrixBuffer", 1, false) }, graphics::EBufferUpdateType::UPDATE_TYPE_CPU);

			// SkinMatは存在するJointの数だけ用意する必要がある
			int SkinMatCount = 1;
			if (Skin) SkinMatCount = static_cast<int>(Skin->GetJointList().size());
			
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

	bool CFBXImporter::CreateMesh(FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		FbxMesh* pFbxMesh = pFBXNode->GetMesh();
		
		if (pFbxMesh)
		{
			// MeshListに登録
			pFbxMeshList.push_back(pFbxMesh);
			
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			{
				// 頂点データに使用するJoint・Weightsを取得する
				// https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/
				std::vector<std::vector<std::pair<unsigned int, float>>> JointWeightPairPerCtrlPoint(pFbxMesh->GetControlPointsCount());

				{
					// 処理中のMeshが関連しているSkinのJointデータを取得する
					unsigned int numOfDeformers = pFbxMesh->GetDeformerCount();
					
					// Deformer(Skin)を取得する
					for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; deformerIndex++)
					{
						// Skin Mesh Animationに使用するDeformerをFbxSkinにキャストして取得
						FbxSkin* pFbxSkin = reinterpret_cast<FbxSkin*>(pFbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

						if (!pFbxSkin) continue;

						// Cluster(Joint)を取得
						unsigned int numOfCluster = pFbxSkin->GetClusterCount();

						for (unsigned int clusterIndex = 0; clusterIndex < numOfCluster; clusterIndex++)
						{
							FbxCluster* pFbxCluster = pFbxSkin->GetCluster(clusterIndex);
							if (!pFbxCluster) continue;

							std::string jointName = pFbxCluster->GetLink()->GetName();
							
							unsigned int JointIndex = FindJointIndexUsingName(Skin, jointName);
							double* Weights = pFbxCluster->GetControlPointWeights(); // このJointを参照している頂点のWeightリスト
							int* VertArrayUsingJoint = pFbxCluster->GetControlPointIndices(); // このJointを参照している頂点のインデックスリスト

							// コントロールポイント == 頂点
							// このJointを参照している頂点の数
							unsigned int VertNumUsingJoint = pFbxCluster->GetControlPointIndicesCount();

							for (unsigned int i = 0; i < VertNumUsingJoint; i++)
							{
								int ControlPointIndex = VertArrayUsingJoint[i];

								std::pair<unsigned int, float> JointWeightPair = { JointIndex , static_cast<float>(Weights[i])};

								JointWeightPairPerCtrlPoint[ControlPointIndex].push_back(JointWeightPair);
							}
						}
					}
				}

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

				// 頂点バッファを読む
				// 参照: https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/
				{
					// 頂点座標
					{
						std::vector<float> AttributePosData;
						std::vector<float> AttributeNormalData;
						std::vector<float> AttributeUVData;
						std::vector<float> AttributeTangentData;
						std::vector<unsigned short> ushort_AttributeJointData;
						std::vector<float> AttributeJointData;
						std::vector<float> AttributeWeightsData;

						int VertexCounter = 0;

						// コントロールポイント == 頂点
						for (int CtrlPointIndex = 0; CtrlPointIndex < pFbxMesh->GetControlPointsCount(); CtrlPointIndex++)
						{
							// 頂点座標
							FbxVector4 pFbxPosition = pFbxMesh->GetControlPointAt(CtrlPointIndex);
							glm::vec3 Pos = glm::vec3(static_cast<float>(pFbxPosition[0]), static_cast<float>(pFbxPosition[1]), static_cast<float>(pFbxPosition[2]));

							// FbxはTranslation・Posが100倍になっているので調整する
							// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
							math::CTransform::CastCentiMeter2Meter(Pos);

							AttributePosData.push_back(Pos.x);
							AttributePosData.push_back(Pos.y);
							AttributePosData.push_back(Pos.z);

							// 法線
							if (pFbxMesh->GetElementNormalCount() > 0)
							{
								FbxGeometryElementNormal* pFbxNormal = pFbxMesh->GetElementNormal(0);
								if (!ReadGeometryElement<FbxGeometryElementNormal>(AttributeNormalData, 3, pFbxNormal, CtrlPointIndex, VertexCounter)) return false;
							}

							// UV
							if (pFbxMesh->GetElementUVCount() > 0)
							{
								FbxGeometryElementUV* pFbxUV = pFbxMesh->GetElementUV(0);
								if (!ReadGeometryElement<FbxGeometryElementUV>(AttributeUVData, 2, pFbxUV, CtrlPointIndex, VertexCounter)) return false;
							}

							// 接線
							if (pFbxMesh->GetElementTangentCount() > 0)
							{
								FbxGeometryElementTangent* pFbxTangent = pFbxMesh->GetElementTangent(0);
								if (!ReadGeometryElement<FbxGeometryElementTangent>(AttributeTangentData, 4, pFbxTangent, CtrlPointIndex, VertexCounter)) return false;
							}

							
							const auto& JointWeightPairList = JointWeightPairPerCtrlPoint[CtrlPointIndex];

							for (int jw = 0; jw < 4; jw++)
							{
								if (jw < JointWeightPairList.size())
								{
									const auto& JointWeightPair = JointWeightPairList[jw];

									// Joint
									ushort_AttributeJointData.push_back(static_cast<unsigned short>(JointWeightPair.first));

									// Weights
									AttributeWeightsData.push_back(JointWeightPair.second);
								}
								else
								{
									// 数が4つより少ない時は0で埋める

									// Joint
									ushort_AttributeJointData.push_back(0);

									// Weights
									AttributeWeightsData.push_back(0.0f);
								}
							}

							// 更新
							VertexCounter++;
						}

						// 頂点座標
						if(!AttributePosData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "POSITION" ,AttributePosData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "POSITION", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "POSITION", 0 });
						}

						// 法線
						if(!AttributeNormalData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "NORMAL" ,AttributeNormalData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "NORMAL", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "NORMAL", 0 });
						}

						// UV
						if (!AttributeUVData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "TEXCOORD_0" ,AttributeUVData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "TEXCOORD_0", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "TEXCOORD_0", 0 });
						}

						// 接線
						if (!AttributeTangentData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "TANGENT" ,AttributeTangentData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "TANGENT", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "TANGENT", 0 });
						}
						else
						{
							// 接線の再計算が必要
							NeedRecalculateTangent = true;
						}

						// Joint

						if (!ushort_AttributeJointData.empty())
						{
							/*size_t size = ushort_AttributeJointData.size() / (sizeof(float) / sizeof(unsigned short));
							AttributeJointData.resize(size);
							std::memcpy(&AttributeJointData[0], &ushort_AttributeJointData[0], sizeof(unsigned short) * ushort_AttributeJointData.size());*/
							
							/*AttributeJointData.resize(ushort_AttributeJointData.size());
							std::transform(ushort_AttributeJointData.begin(), ushort_AttributeJointData.end(), AttributeJointData.begin(), [](unsigned short val) { return static_cast<float>(val); });*/

							std::vector<unsigned char> BufferData;
							BufferData.resize(sizeof(unsigned short) * ushort_AttributeJointData.size());
							std::memcpy(&BufferData[0], &ushort_AttributeJointData[0], sizeof(unsigned short) * ushort_AttributeJointData.size());

							AttributeJointData.resize(BufferData.size() / sizeof(unsigned short));
							std::memcpy(&AttributeJointData[0], &BufferData[0], BufferData.size());

							// データを登録
							ReservedVertexDataList.insert({ "JOINTS_0" ,AttributeJointData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_SHORT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "JOINTS_0", 8 });
						}

						// Weights
						if (!AttributeWeightsData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "WEIGHTS_0" ,AttributeWeightsData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "WEIGHTS_0", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "WEIGHTS_0", 0 });
						}
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
							// 頂点数
							int VertexCount = pFbxMesh->GetControlPointsCount();

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

				// タンジェントの再計算
				if (NeedRecalculateTangent)
				{
					if (Indices.size() > 0)
					{
						if (!RecalculateTangent(ReservedVertexDataList["TANGENT"], ReservedVertexDataList["POSITION"], ReservedVertexDataList["TEXCOORD_0"], Indices)) return false;
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

	bool CFBXImporter::CreateNodeList(FbxScene* Scene, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		// ルートノードを取得
		FbxNode* RootNode = Scene->GetRootNode();
		if (RootNode)
		{
			RootNodeIndexList.push_back(std::vector<int>(1, 0));

			if (!CreateNode(RootNode, pFbxNodeList, NodeList)) return false;
		}

		// 子要素を登録する
		if (pFbxNodeList.size() != NodeList.size()) return false;

		for (int n = 0; n < NodeList.size(); n++)
		{
			FbxNode* pFBXNode = pFbxNodeList[n];
			const auto& Node = NodeList[n];
			
			std::vector<int> ChildNodeList;

			for (int i = 0; i < pFBXNode->GetChildCount(); i++)
			{
				FbxNode* pChildNode = pFBXNode->GetChild(i);

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

	bool CFBXImporter::CreateNode(FbxNode* pFBXNode, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		// Nodeを作成
		// MeshとSkinは後ほどセットする
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(-1, std::vector<std::shared_ptr<graphics::CMesh>>(), std::vector<std::shared_ptr<graphics::CMaterial>>());

		std::string NodeName = std::string(pFBXNode->GetName());
		Node->SetName(NodeName);

		glm::vec3 Pos = glm::vec3(0.0f);
		glm::quat Rotation = glm::quat(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		glm::vec3 Scale = glm::vec3(1.0f);

		if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			glm::mat4 LocalMatrix = glm::mat4(1.0f);

			FbxAMatrix fbxMat = pFBXNode->EvaluateLocalTransform();
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					LocalMatrix[row][col] = static_cast<float>(fbxMat[row][col]);
				}
			}

			math::CTransform::CastModelMatrixToTransform(LocalMatrix, Pos, Rotation, Scale);
		}
		else
		{
			FbxDouble3 fbxTranslation = pFBXNode->LclTranslation.Get();
			FbxDouble3 fbxRotation = pFBXNode->LclRotation.Get();
			FbxDouble3 fbxScale = pFBXNode->LclScaling.Get();

			Pos = glm::vec3(static_cast<float>(fbxTranslation[0]), static_cast<float>(fbxTranslation[1]), static_cast<float>(fbxTranslation[2]));
			Rotation =
				glm::angleAxis(static_cast<float>(fbxRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::angleAxis(static_cast<float>(fbxRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::angleAxis(static_cast<float>(fbxRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
			Scale = glm::vec3(static_cast<float>(fbxScale[0]), static_cast<float>(fbxScale[1]), static_cast<float>(fbxScale[2]));
		}

		// FbxはTranslation・Posが100倍になっているので調整する
		// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
		math::CTransform::CastCentiMeter2Meter(Pos);

		math::CTransform::ToYUpRightHandedCoordinate(Pos);
		math::CTransform::ToYUpRightHandedCoordinate(Rotation);

		Node->SetPos(Pos);
		Node->SetRot(Rotation);
		Node->SetScale(Scale);

		// ノードを登録
		NodeList.push_back(Node);

		pFbxNodeList.push_back(pFBXNode);

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateNode(pFBXNode->GetChild(i), pFbxNodeList, NodeList)) return false;
		}

		return true;
	}

	bool CFBXImporter::ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<FbxNode*>& pFbxNodeList, const std::vector<FbxMesh*>& pFbxMeshList,
		const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		// NodeListとpFbxNodeListは同じ順番で同じ数
		if (NodeList.size() != pFbxNodeList.size()) return false;

		for (int i = 0; i < NodeList.size(); i++)
		{
			auto& Node = NodeList[i];
			FbxNode* pFBXNode = pFbxNodeList[i];

			// Listの要素のポインターとpFBXNodeが持ってる要素のポインターを比較してリスト内の順番をIndexとして渡す
			int MeshIndex = -1;
			int Loop = 0;
			for (auto it = pFbxMeshList.begin(); it != pFbxMeshList.end(); it++)
			{
				FbxMesh* pTargetMesh = *it;
				if (pTargetMesh && pTargetMesh == pFBXNode->GetMesh())
				{
					MeshIndex = Loop;

					break;
				}

				Loop++;
			}

			// MeshIndexを設定
			Node->SetMeshIndexWithDynamicOffset(MeshIndex, MeshList, MaterialList);

			// JointがあるならSkinが1つあるとする
			int SkinIndex = (Skin->GetJointList().size() > 0)? 0 : - 1;
			Node->SetSkinIndex(SkinIndex);
		}

		return true;
	}

	bool CFBXImporter::CreateAnimationSkin(FbxNode* pFBXNode, std::shared_ptr<animation::CSkin>& Skin, std::vector<FbxNode*>& FbxJointList, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			auto JointNode = GetJointNode(pFBXNode->GetName(), NodeList);

			std::shared_ptr<animation::CJoint> Joint = std::make_shared<animation::CJoint>(JointNode);

			// SkinのInverseBindMatrixを作成
			glm::mat4 InverseBindMatrix = glm::inverse(Joint->GetJointNode()->GetWorldMatrix());
			Joint->GetJointNode()->SetInverseBindMatrix(InverseBindMatrix);

			// BoneNameを取得
			std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
			animation::EHumanoidBones BoneName = Provider->GetBoneName(pFBXNode->GetName());

			// JointにBoneNameを割り当てる
			Joint->SetBoneName(BoneName);

			Skin->AddJoint(Joint);

			// FbxJointListを登録
			FbxJointList.push_back(pFBXNode);
		}

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateAnimationSkin(pFBXNode->GetChild(i), Skin, FbxJointList, NodeList)) return false;
		}

		return true;
	}

	void CFBXImporter::ApplyParentJointList(const std::shared_ptr<animation::CSkin>& Skin, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& Joint : Skin->GetJointList())
		{
			const auto& ParentNode = Joint->GetJointNode()->GetParentNode();
			if (!ParentNode) continue;

			std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
			animation::EHumanoidBones ParentBoneName = Provider->GetBoneName(ParentNode->GetName());

			const auto& ParentJoint = Skin->GetBone(ParentBoneName);
			if (!ParentJoint) continue;

			Joint->SetParentBoneName(ParentJoint->GetBoneName());
		}
	}

	bool CFBXImporter::CreateAnimation(FbxScene* Scene, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::shared_ptr<animation::CSkin>& Skin, const std::vector<FbxNode*>& FbxJointList)
	{

		for (int i = 0; i < Scene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			std::shared_ptr<animation::CAnimationClip> AnimationClip = std::make_shared<animation::CAnimationClip>();

			// AnimStackはアニメーションクリップのようなもの
			FbxAnimStack* pAnimStack = Scene->GetSrcObject<FbxAnimStack>(i);
			FbxString animStackName = pAnimStack->GetName();

			// samplers
			{
				std::vector<std::shared_ptr<animation::CAnimationSampler>> AnimationSamplerList;

				FbxTakeInfo* takeInfo = Scene->GetTakeInfo(animStackName);
				fbxsdk::FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart(); // キーフレームの最初の時間
				fbxsdk::FbxTime endTime = takeInfo->mLocalTimeSpan.GetStop();    // キーフレームの終わりの時間

				fbxsdk::FbxGlobalSettings& GloabalSettions = Scene->GetGlobalSettings();
				fbxsdk::FbxTime::EMode fileTimeMode = GloabalSettions.GetTimeMode();

				for (const auto& pFbxJoint : FbxJointList) 
				{ 
					// ひとまず全部LINEARにしておく
					std::shared_ptr<animation::CAnimationSampler> Sampler = std::make_shared<animation::CAnimationSampler>(animation::EInterpolationType::LINEAR);

					// 始めの時間と終わりの時間を指定する
					Sampler->SetStartTime(static_cast<float>(startTime.GetSecondDouble()));
					Sampler->SetEndTime(static_cast<float>(endTime.GetSecondDouble()));

					AnimationSamplerList.push_back(Sampler);
				}

				// 30 FPS換算のフレーム数を取得し、EndとStartとの差分から実際に使用されるフレーム数を計算
				FbxLongLong AnimationLength = endTime.GetFrameCount(fileTimeMode) - startTime.GetFrameCount(fileTimeMode) + 1;

				// FBXにはchannelといった概念はなく、Translation・Rotation・Scaleを全てまとめてModelMatrixで計算している
				// なのでChannelTypeにFBX-SDK限定の値としてMODELMATRIXを作成することで対応する
				for (FbxLongLong FrameIndex = startTime.GetFrameCount(fileTimeMode); FrameIndex <= endTime.GetFrameCount(fileTimeMode); FrameIndex++)
				{
					FbxTime currentTime;
					currentTime.SetFrame(FrameIndex, fileTimeMode);

					// pFbxJointListとCSkinからアニメーション情報を取得する
					for (int JointIndex = 0; JointIndex < FbxJointList.size(); JointIndex++)
					{
						const auto& pFbxJoint = FbxJointList[JointIndex];
						if (!pFbxJoint) return false;

						glm::mat4 CurrentMatrix = glm::mat4(1.0f);

						FbxAMatrix fbxMat = pFbxJoint->EvaluateLocalTransform(currentTime);
						for (int row = 0; row < 4; row++)
						{
							for (int col = 0; col < 4; col++)
							{
								CurrentMatrix[row][col] = static_cast<float>(fbxMat[row][col]);
							}
						}

						// FBXの値を整形する
						{
							glm::vec3 Pos = glm::vec3(0.0f);
							glm::quat Rotation = glm::quat(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
							glm::vec3 Scale = glm::vec3(1.0f);

							math::CTransform::CastModelMatrixToTransform(CurrentMatrix, Pos, Rotation, Scale);

							// FbxはTranslation・Posが100倍になっているので調整する
							// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
							math::CTransform::CastCentiMeter2Meter(Pos);

							math::CTransform::ToYUpRightHandedCoordinate(Pos);
							math::CTransform::ToYUpRightHandedCoordinate(Rotation);

							math::CTransform::CalcModelMatrix(CurrentMatrix, Pos, Rotation, Scale);
						}
						
						// Input
						float InputData = static_cast<float>(currentTime.GetSecondDouble());
						
						// Output
						glm::mat4 OutputMat = CurrentMatrix;
						std::vector<float> OutputData(16);
						std::memcpy(&OutputData[0], &OutputMat[0][0], sizeof(glm::mat4));

						// Create KeyFrame
						// FBX SDKでは全てKEYFRAME_TYPE_MATRIX
						std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(animation::EKeyFrameType::KEYFRAME_TYPE_MATRIX);

						KeyFrame->SetInput(InputData);
						KeyFrame->SetOutput(OutputData);

						// Add KeyFrame To Sampler
						if (AnimationSamplerList[JointIndex]->GetKeyFrameList().size() < static_cast<size_t>(AnimationLength))
						{
							AnimationSamplerList[JointIndex]->AddKeyFrame(KeyFrame);
						}
					}
				}

				// SamplerListを渡す
				for (const auto& Sampler : AnimationSamplerList)
				{
					// 一番最後のKeyFrameの時間をEndTimeにする
					Sampler->SetEndTime(Sampler->GetKeyFrameList()[Sampler->GetKeyFrameList().size() - 1]->GetInput());

					AnimationClip->AddAnimationSampler(Sampler);
				}
			}

			// channels
			for (int JointIndex = 0; JointIndex < FbxJointList.size(); JointIndex++)
			{
				const auto& pFbxJoint = FbxJointList[JointIndex];

				// FBXにはchannelといった概念はなく、Translation・Rotation・Scaleを全てまとめてModelMatrixで計算している
				// なのでChannelTypeにFBX-SDK限定の値としてMODELMATRIXを作成することで対応する
				animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::MODELMATRIX;

				// Jointの順番と参照するSamplerの順番は同じである
				int TargetSamplerIndex = JointIndex;

				std::string JointName = pFbxJoint->GetName();

				// アニメーションのターゲットを取得する
				const auto& TargetNode = GetJointNode(JointName, NodeList);

				// Bone Name を取得
				std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
				animation::EHumanoidBones BoneName = Provider->GetBoneName(JointName);

				std::shared_ptr<animation::CAnimationChannel> AnimationChannel = std::make_shared<animation::CAnimationChannel>(TargetSamplerIndex, AnimationTarget, TargetNode, BoneName);

				AnimationClip->AddAnimationChannel(AnimationChannel);
			}

			//
			AnimationClip->SetDefaultSkin(Skin);

			AnimationClipList.push_back(AnimationClip);
		}

		return true;
	}

	// Helper Function //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	bool CFBXImporter::ReadGeometryElement(std::vector<float>& AttributeData, int Dimention, T* pFbxElement, int CtrlPointIndex, int VertexCounter)
	{
		// inCtrlPointIndex : コントロール ポイントのインデックス
		// inVertexCounter  : 処理している現在の頂点のインデックス

		// どこからバッファを受け取るのか
		switch (pFbxElement->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			// eByControlPoint : 制御点のインデックスによって制御点の法線を指定

			// どのような形式でバッファを参照するのか
			switch (pFbxElement->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				// FbxGeometryElement::eDirect は、制御点のインデックスまたは面頂点のインデックスを直接使用してElementを参照できることを意味する
				for (int i = 0; i < Dimention; i++)
				{
					AttributeData.push_back(static_cast<float>(pFbxElement->GetDirectArray().GetAt(CtrlPointIndex).mData[i]));
				}
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				// FbxGeometryElement::eIndexToDirect は、制御点のインデックスまたは面頂点のインデックスを使用すると、必要なElementを指すインデックスのみが得られるため、
				// 実際のElementを見つけるにはこのインデックスを使用する必要があることを意味
				for (int i = 0; i < Dimention; i++)
				{
					int Index = pFbxElement->GetIndexArray().GetAt(CtrlPointIndex);
					AttributeData.push_back(static_cast<float>(pFbxElement->GetDirectArray().GetAt(Index).mData[i]));
				}
			}
			break;

			default:
				return false;
			}
		}
		break;

		case FbxGeometryElement::eByPolygonVertex:
		{
			// eByPolygonVertex : 頂点のインデックスによって面上の頂点のElementを指定

			// どのような形式でバッファを参照するのか
			switch (pFbxElement->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				// FbxGeometryElement::eDirect は、制御点のインデックスまたは面頂点のインデックスを直接使用してElementを参照できることを意味する
				for (int i = 0; i < Dimention; i++)
				{
					AttributeData.push_back(static_cast<float>(pFbxElement->GetDirectArray().GetAt(VertexCounter).mData[i]));
				}
			}
			break;

			case FbxGeometryElement::eIndexToDirect:
			{
				// FbxGeometryElement::eIndexToDirect は、制御点のインデックスまたは面頂点のインデックスを使用すると、必要なElementを指すインデックスのみが得られるため、
				// 実際のElementを見つけるにはこのインデックスを使用する必要があることを意味
				for (int i = 0; i < Dimention; i++)
				{
					int Index = pFbxElement->GetIndexArray().GetAt(VertexCounter);
					AttributeData.push_back(static_cast<float>(pFbxElement->GetDirectArray().GetAt(Index).mData[i]));
				}
			}
			break;

			default:
				return false;
			}
		}
		break;

		default:
			return false;
		}

		return true;
	}

	bool CFBXImporter::RecalculateTangent(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices)
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

	std::shared_ptr<object::CNode> CFBXImporter::GetJointNode(const std::string& JointName, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		std::shared_ptr<object::CNode> JointNode = nullptr;

		for (const auto& Node : NodeList)
		{
			if (Node->GetName() == JointName)
			{
				JointNode = Node;

				break;
			}
		}

		return JointNode;
	}

	unsigned int CFBXImporter::FindJointIndexUsingName(const std::shared_ptr<animation::CSkin>& Skin, const std::string& JointName)
	{
		unsigned int JointIndex = 0;

		for (int j = 0; j < Skin->GetJointList().size(); j++)
		{
			const auto& Joint = Skin->GetJointList()[j];

			if (Joint->GetJointNode()->GetName() == JointName)
			{
				JointIndex = j;

				break;
			}
		}

		return JointIndex;
	}
}
#endif // USE_FBX