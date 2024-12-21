#if defined(USE_FBX) && !defined(USE_SMALL_FBX)
#include "CFBXImporter.h"
#include "CFBXStream.h"
#include "CFBXMomoryStream.h"
#include "../Animation/CBoneNameProvider.h"

#include "../Object/C3DObject.h"

#include "../Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkeleton.h"
#include "../Animation/CBone.h"

#include "../Graphics/CMaterialFrame.h"
#include "../../Graphics/CVertexBuffer.h"
#include "../../Graphics/CIndexBuffer.h"

#include "../../LoadWorker/CLoadWorker.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../LoadWorker/CTextureLoader.h"

using namespace fbxsdk;

namespace fbx
{
	bool CFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, object::C3DObject* Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, resource::C3DObjectLoader* p3DObjectLoader, 
		animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		if (!Import(pGraphicsAPI, FileName, true, Object, AnimationClipList, BaseMaterialFrameList, RigType, HumanoidBoneList)) return false;

		return true;
	}

	bool CFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, 
		animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
	{
		std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>();

		Object->SetObjectName(FileName);

		if (!Import(pGraphicsAPI, FileName, false, Object.get(), AnimationClipList, std::vector<std::shared_ptr<graphics::CMaterialFrame>>(), 
			RigType, HumanoidBoneList)) return false;

		return true;
	}

	bool CFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
	{
		// 全体のメモリやObjectを管理するManagerを作成
		FbxManager* Manager = FbxManager::Create();

		// IO Object(Input/Output)を作成
		FbxIOSettings* ios = FbxIOSettings::Create(Manager, IOSROOT);
		Manager->SetIOSettings(ios);

		// Importerを生成
		FbxImporter* Importer = FbxImporter::Create(Manager, "");

		// Streamを作成
		//CFBXStream Stream(Manager, "rb", "Resources\\Motions\\Walking_WithSkeleton.fbx");
		//CFBXMomoryStream Stream(Manager, Data);
		//void* streamData = NULL;

		// Importerを初期化
		if (!Importer->Initialize(FileName.c_str(), -1, Manager->GetIOSettings()))
		//if (!Importer->Initialize(&Stream, streamData, -1, Manager->GetIOSettings()))
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
		if (!Analyse(pGraphicsAPI, Scene, IsUseObject, Object, AnimationClipList, BaseMaterialFrameList, RigType, HumanoidBoneList)) return false;

		// FBX解析を終了
		Manager->Destroy();

		return true;
	}

	bool CFBXImporter::Analyse(api::IGraphicsAPI* pGraphicsAPI, FbxScene* Scene, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
	{
		FbxNode* RootNode = Scene->GetRootNode();

		// MixamoのFbxかどうか. MixamoのデータはPosの単位やRoationが特殊なので内部的に色々と補正する必要がある
		const bool IsMixamoFbx = CheckIsMixamo(RootNode);

		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<int> RootNodeIndexList;
		std::vector<FbxNode*> pFbxNodeList;

		if (!CreateNodeList(Scene, pFbxNodeList, NodeList, RootNodeIndexList, IsMixamoFbx)) return false;

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

		// Skeleton
		std::shared_ptr<animation::CSkeleton> Skeleton = std::make_shared<animation::CSkeleton>(RigType, (Object->GetObjectName() + "(Skeleton)"));
		std::vector<FbxNode*> FbxBoneList;
		if (RootNode)
		{
			if (!CreateAnimationSkeleton(pGraphicsAPI, RootNode, Skeleton, FbxBoneList, NodeList, IsMixamoFbx)) return false;
		}

		Object->SetAnimationSkeleton(Skeleton);

		// BoneTableを作成
		Skeleton->MakeHumanoidBoneTable(HumanoidBoneList);

		// DefaultLocalTransformを保存する
		Object->ApplyDefaultLocalTransform();

		// 親ノードを設定
		Object->ApplyParentNode();

		// ワールド行列の計算
		Object->CalcWorldMatrix();

		// 親のBoneを追加
		ApplyParentBoneList(pGraphicsAPI, Skeleton, NodeList);

		// アニメーション
		if (!CreateAnimation(pGraphicsAPI, Scene, AnimationClipList, NodeList, Skeleton, FbxBoneList, IsMixamoFbx)) return false;

		if (IsUseObject)
		{
			// マテリアルはプリミティブ単位で生成する
			std::vector<std::vector<std::tuple<std::shared_ptr<graphics::CMaterialFrame>, std::shared_ptr<graphics::CMaterial>>>> BaseMaterialList;
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			std::vector<std::shared_ptr<graphics::CMesh>> MeshList;

			if (RootNode)
			{
				// 描画情報の取得
				std::vector<FbxMesh*> pFbxMeshList;
				if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, BaseMaterialFrameList, pFbxNodeList, RootNode, TextureList, BaseMaterialList, MeshList, Skeleton, IsMixamoFbx)) return false;

				// マテリアルを持っていないのならダミーを渡す
				if (BaseMaterialList.size() <= 0)
				{
					if (!CreateDummyMaterial(pGraphicsAPI, RootNode, BaseMaterialFrameList, MeshList, Skeleton)) return false;
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
				if (!ConnectNodeTo(NodeList, pFbxNodeList, pFbxMeshList, Skeleton)) return false;
			}

			// オブジェクトにリソースを登録
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
				Object->AddAnimationClip(Clip, "Default", { nullptr, "" }, true);
			}
		}

		return true;
	}

	bool CFBXImporter::CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<FbxMesh*>& pFbxMeshList, const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
		const std::vector<FbxNode*>& pFbxNodeList, FbxNode* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
		std::vector<std::vector<std::tuple<std::shared_ptr<graphics::CMaterialFrame>, std::shared_ptr<graphics::CMaterial>>>>& BaseMaterialList,
		std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx)
	{
		int NodeIndex = -1;
		const auto it = std::find(pFbxNodeList.begin(), pFbxNodeList.end(), pFBXNode);
		if (it != pFbxNodeList.end())
		{
			NodeIndex = static_cast<int>(it - pFbxNodeList.begin());
		}

		// 知りたいのは描画情報なのでここではeMeshのみ見る
		if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			std::vector<FbxSurfaceMaterial*> pFbxMaterialList;

			// テクスチャ

			// マテリアル
			if (!CreateMaterial(pGraphicsAPI, pFBXNode, pFbxMaterialList, BaseMaterialList, BaseMaterialFrameList, Skeleton)) return false;

			// メッシュ
			if (!CreateMesh(pGraphicsAPI, NodeIndex, pFBXNode, pFbxMeshList, pFbxMaterialList, MeshList, BaseMaterialList, Skeleton, IsMixamoFbx)) return false;
		}
		

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, BaseMaterialFrameList, pFbxNodeList, pFBXNode->GetChild(i), TextureList,
				BaseMaterialList, MeshList, Skeleton, IsMixamoFbx)) return false;
		}

		return true;
	}

	bool CFBXImporter::CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::vector<FbxSurfaceMaterial*>& pFbxMaterialList,
		std::vector<std::vector<std::tuple<std::shared_ptr<graphics::CMaterialFrame>, std::shared_ptr<graphics::CMaterial>>>>& BaseMaterialList,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		for (int m = 0; m < pFBXNode->GetMaterialCount(); m++)
		{
			fbxsdk::FbxSurfaceMaterial* pFbxMaterial = pFBXNode->GetMaterial(m);
			if (!pFbxMaterial) continue;

			pFbxMaterialList.push_back(pFbxMaterial);

			// プリミティブ単位で割り当てられるマテリアルリスト
			std::vector<std::tuple<std::shared_ptr<graphics::CMaterialFrame>, std::shared_ptr<graphics::CMaterial>>> PrimitiveMaterials;

			// マテリアルにシェーダーを設定
			for (const auto& MaterialFrame : BaseMaterialFrameList)
			{
				std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_BACK);

				{
					const auto& prop = pFbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
					if (prop.IsValid())
					{
						const auto& val = prop.Get<fbxsdk::FbxDouble3>();
						//material->ReplacePreloadUniformValue("baseColorFactor", &glm::vec4(static_cast<float>(val[0]), static_cast<float>(val[1]), static_cast<float>(val[2]), 1.0f)[0], sizeof(glm::vec4), 0);
					}
				}

				// SkinMatrix StorageBuffer
				{
					// SkinMatは存在するBoneの数だけ用意する必要がある
					int SkinMatCount = 1;
					if (Skeleton) SkinMatCount = static_cast<int>(Skeleton->GetBoneList().size());

					// DynamicOffsetが256バイトからしか使えない都合上SkinMatCountの最小値は4とする(4 * 16 * 4 = 256)
					if (SkinMatCount < 4) SkinMatCount = 4;

					// SSBOのサイズは2のn乗である必要がある
					SkinMatCount = math::CMath::CalcNextPowerOfTwo(SkinMatCount);

					// 最大ボーン数よりも多いのならエラーとする
					if (SkinMatCount > pGraphicsAPI->GetMaxBoneCount())
					{
						Console::Log("[Error] SkinMatCount is over MaxBoneCount. - SkinMatCount: %d, MaxBoneCount: %d\n", SkinMatCount, pGraphicsAPI->GetMaxBoneCount());
						return false;
					}

					std::vector<glm::mat4> SkinMatrixList;
					SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

					material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
				}

				PrimitiveMaterials.push_back(std::make_pair(MaterialFrame, material));
			}

			BaseMaterialList.push_back(PrimitiveMaterials);
		}

		return true;
	}

	bool CFBXImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
		const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		if (BaseMaterialFrameList.empty()) return true;

		// マテリアル参照数とマテリアルインデックスの設定
		for (auto& Mesh : MeshList)
		{
			for (auto& Primirive : Mesh->GetPrimitiveList())
			{
				for (const auto& MaterialFrame : BaseMaterialFrameList)
				{
					// マテリアルにシェーダーを設定
					std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_NONE);

					// SkinMatrix StorageBuffer
					{
						// SkinMatは存在するBoneの数だけ用意する必要がある
						int SkinMatCount = 1;
						if (Skeleton) SkinMatCount = static_cast<int>(Skeleton->GetBoneList().size());

						// SSBOのサイズは2のn乗である必要がある
						SkinMatCount = math::CMath::CalcNextPowerOfTwo(SkinMatCount);

						// 最大ボーン数よりも多いのならエラーとする
						if (SkinMatCount > pGraphicsAPI->GetMaxBoneCount())
						{
							Console::Log("[Error] SkinMatCount is over MaxBoneCount. - SkinMatCount: %d, MaxBoneCount: %d\n", SkinMatCount, pGraphicsAPI->GetMaxBoneCount());
							return false;
						}

						std::vector<glm::mat4> SkinMatrixList;
						SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

						material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
					}

					Primirive->AddMaterial(pGraphicsAPI, material);
				}
			}
		}

		return true;
	}

	bool CFBXImporter::CreateMesh(api::IGraphicsAPI* pGraphicsAPI, int NodeIndex, FbxNode* pFBXNode, std::vector<FbxMesh*>& pFbxMeshList,
		const std::vector<FbxSurfaceMaterial*>& pFbxMaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList,
		const std::vector<std::vector<std::tuple<std::shared_ptr<graphics::CMaterialFrame>, std::shared_ptr<graphics::CMaterial>>>>& BaseMaterialList,
		const std::shared_ptr<animation::CSkeleton>& Skeleton, const bool IsMixamoFbx)
	{
		FbxMesh* pFbxMesh = pFBXNode->GetMesh();
		
		if (pFbxMesh)
		{
			// FBXMaterialIndexを取得
			int FBXMaterialIndex = -1;
			{
				int MatCount = pFbxMesh->GetElementMaterialCount();

				if (MatCount != 0)
				{
					fbxsdk::FbxLayerElementMaterial* pFbxElementMaterial = pFbxMesh->GetElementMaterial(0);
					
					int Index = pFbxElementMaterial->GetIndexArray().GetAt(0);
					FbxSurfaceMaterial* pFbxSurfaceMaterial = pFbxMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(Index);

					for (size_t i = 0; i < pFbxMaterialList.size(); i++)
					{
						if (pFbxSurfaceMaterial == pFbxMaterialList[i])
						{
							FBXMaterialIndex = static_cast<int>(BaseMaterialList.size() - pFbxMaterialList.size() + i);

							if (FBXMaterialIndex < 0 || FBXMaterialIndex >= static_cast<int>(BaseMaterialList.size())) return false;

							break;
						}
					}
				}
			}

			// MeshListに登録
			pFbxMeshList.push_back(pFbxMesh);
			
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			{
				// 頂点データに使用するBone・Weightsを取得する
				// https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/
				std::vector<std::vector<std::pair<unsigned int, float>>> BoneWeightPairPerCtrlPoint(pFbxMesh->GetControlPointsCount());

				{
					// 処理中のMeshが関連しているSkeletonのBoneデータを取得する
					unsigned int numOfDeformers = pFbxMesh->GetDeformerCount();
					
					// Deformer(Skeleton)を取得する
					for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; deformerIndex++)
					{
						// Skeleton Mesh Animationに使用するDeformerをFbxSkinにキャストして取得
						FbxSkin* pFbxSkin = reinterpret_cast<FbxSkin*>(pFbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

						if (!pFbxSkin) continue;

						// Cluster(Bone)を取得
						unsigned int numOfCluster = pFbxSkin->GetClusterCount();

						for (unsigned int clusterIndex = 0; clusterIndex < numOfCluster; clusterIndex++)
						{
							FbxCluster* pFbxCluster = pFbxSkin->GetCluster(clusterIndex);
							if (!pFbxCluster) continue;

							std::string BoneName = pFbxCluster->GetLink()->GetName();
							
							unsigned int BoneIndex = FindBoneIndexUsingName(Skeleton, BoneName);
							double* Weights = pFbxCluster->GetControlPointWeights(); // このBoneを参照している頂点のWeightリスト
							int* VertArrayUsingBone = pFbxCluster->GetControlPointIndices(); // このBoneを参照している頂点のインデックスリスト

							// コントロールポイント == 頂点
							// このBoneを参照している頂点の数
							unsigned int VertNumUsingBone = pFbxCluster->GetControlPointIndicesCount();

							for (unsigned int i = 0; i < VertNumUsingBone; i++)
							{
								int ControlPointIndex = VertArrayUsingBone[i];

								std::pair<unsigned int, float> BoneWeightPair = { BoneIndex , static_cast<float>(Weights[i])};

								BoneWeightPairPerCtrlPoint[ControlPointIndex].push_back(BoneWeightPair);
							}
						}
					}
				}

				// 頂点バッファ本体
				std::vector<std::vector<float>> VertexDataList;
				std::vector<int> DimentionList;
				std::vector<graphics::EDataType> DataTypeList;
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
				std::map<std::string, graphics::EDataType> ReservedDataTypeList;
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
						std::vector<unsigned short> ushort_AttributeBoneData;
						std::vector<float> AttributeBoneData;
						std::vector<float> AttributeWeightsData;

						int VertexCounter = 0;

						// コントロールポイント == 頂点
						for (int CtrlPointIndex = 0; CtrlPointIndex < pFbxMesh->GetControlPointsCount(); CtrlPointIndex++)
						{
							// 頂点座標
							FbxVector4 pFbxPosition = pFbxMesh->GetControlPointAt(CtrlPointIndex);
							glm::vec3 Pos = glm::vec3(static_cast<float>(pFbxPosition[0]), static_cast<float>(pFbxPosition[1]), static_cast<float>(pFbxPosition[2]));

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

							
							const auto& BoneWeightPairList = BoneWeightPairPerCtrlPoint[CtrlPointIndex];

							for (int jw = 0; jw < 4; jw++)
							{
								if (jw < BoneWeightPairList.size())
								{
									const auto& BoneWeightPair = BoneWeightPairList[jw];

									// Bone
									ushort_AttributeBoneData.push_back(static_cast<unsigned short>(BoneWeightPair.first));

									// Weights
									AttributeWeightsData.push_back(BoneWeightPair.second);
								}
								else
								{
									// 数が4つより少ない時は0に設定する
									ushort_AttributeBoneData.push_back(0); // Bone
									AttributeWeightsData.push_back(0.0f); // Weights
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
							ReservedDataTypeList.insert({ "POSITION", graphics::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "POSITION", 0 });
						}

						// 法線
						if(!AttributeNormalData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "NORMAL" ,AttributeNormalData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "NORMAL", graphics::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "NORMAL", 0 });
						}

						// UV
						if (!AttributeUVData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "TEXCOORD_0" ,AttributeUVData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "TEXCOORD_0", graphics::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "TEXCOORD_0", 0 });
						}

						// 接線
						if (!AttributeTangentData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "TANGENT" ,AttributeTangentData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "TANGENT", graphics::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "TANGENT", 0 });
						}
						else
						{
							// 接線の再計算が必要
							NeedRecalculateTangent = true;
						}

						// Bone

						if (!ushort_AttributeBoneData.empty())
						{
							/*size_t size = ushort_AttributeBoneData.size() / (sizeof(float) / sizeof(unsigned short));
							AttributeBoneData.resize(size);
							std::memcpy(&AttributeBoneData[0], &ushort_AttributeBoneData[0], sizeof(unsigned short) * ushort_AttributeBoneData.size());*/
							
							/*AttributeBoneData.resize(ushort_AttributeBoneData.size());
							std::transform(ushort_AttributeBoneData.begin(), ushort_AttributeBoneData.end(), AttributeBoneData.begin(), [](unsigned short val) { return static_cast<float>(val); });*/

							std::vector<unsigned char> BufferData;
							BufferData.resize(sizeof(unsigned short) * ushort_AttributeBoneData.size());
							std::memcpy(&BufferData[0], &ushort_AttributeBoneData[0], sizeof(unsigned short) * ushort_AttributeBoneData.size());

							AttributeBoneData.resize(BufferData.size() / sizeof(unsigned short));
							std::memcpy(&AttributeBoneData[0], &BufferData[0], BufferData.size());

							// データを登録
							ReservedVertexDataList.insert({ "JOINTS_0" ,AttributeBoneData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "JOINTS_0", graphics::EDataType::TYPE_UNSIGNED_SHORT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "JOINTS_0", 8 });
						}

						// Weights
						if (!AttributeWeightsData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "WEIGHTS_0" ,AttributeWeightsData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "WEIGHTS_0", graphics::EDataType::TYPE_FLOAT });

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

							if (AttribName == "JOINTS_0")
							{
								// BoneWeightsが存在しない時はそのメッシュを持つノードのインデックスを参照するようにする
								ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexCount * Dimention, static_cast<float>(NodeIndex)) });
							}
							else if (AttribName == "WEIGHTS_0")
							{
								// BoneWeightsが存在しない時はそのメッシュを持つノードのインデックスを参照するようにする
								ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexCount * Dimention, 1.0f) });
							}
							else
							{
								ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexCount * Dimention, 0.0f) });
							}

							

							// 接線もしく複接線の再計算が必要
							if (AttribName == "TANGENT")
							{
								NeedRecalculateTangent = true;
							}

							// DataTypeとByteStrideの初期値をセット
							graphics::EDataType DataType = graphics::EDataType::TYPE_FLOAT;

							// 『JOINTS_0』はunsigned shortである
							if (AttribName == "JOINTS_0") DataType = graphics::EDataType::TYPE_UNSIGNED_SHORT;

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

				// 頂点バッファを作成する
				auto VertexBuffer = pGraphicsAPI->CreateVertexBuffer();

				// メッシュ情報を渡す
				VertexBuffer->SetVertices(VertexDataList);
				VertexBuffer->SetAttributeDimensions(DimentionList);
				VertexBuffer->SetAttribDataTypes(DataTypeList);
				VertexBuffer->SetAttribByteStrides(ByteStrideList);

				Mesh->AddVertexBuffer(VertexBuffer);

				//
				auto IndexBuffer = pGraphicsAPI->CreateIndexBuffer();

				// Indicesを登録
				IndexBuffer->SetIndices(Indices);

				Mesh->AddIndexBuffer(IndexBuffer);

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer);

				// マテリアルはプリミティブ単位で生成する
				// プリミティブ単位で割り当てられるマテリアルリスト
				const auto& PrimitiveMaterials = BaseMaterialList[FBXMaterialIndex];
				for (const auto& PrimitiveMat : PrimitiveMaterials)
				{
					auto NewMaterial = std::get<0>(PrimitiveMat)->CopyMaterial(pGraphicsAPI, std::get<1>(PrimitiveMat));
					if (!NewMaterial) return false;

					Primitive->AddMaterial(pGraphicsAPI, NewMaterial);
				}

				Mesh->AddPrimitive(Primitive);
			}

			MeshList.push_back(Mesh);
		}

		return true;
	}

	bool CFBXImporter::CreateNodeList(FbxScene* Scene, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<int>& RootNodeIndexList, const bool IsMixamoFbx)
	{
		// ルートノードを取得
		FbxNode* RootNode = Scene->GetRootNode();
		if (RootNode)
		{
			RootNodeIndexList.push_back(0);

			if (!CreateNode(RootNode, pFbxNodeList, NodeList, IsMixamoFbx)) return false;
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

	bool CFBXImporter::CreateNode(FbxNode* pFBXNode, std::vector<FbxNode*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, const bool IsMixamoFbx)
	{
		// Nodeを作成
		// MeshとSkeletonは後ほどセットする
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));

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

		Node->SetPos(Pos);
		Node->SetRot(Rotation);
		Node->SetScale(Scale);

		// ノードを登録
		NodeList.push_back(Node);

		pFbxNodeList.push_back(pFBXNode);

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateNode(pFBXNode->GetChild(i), pFbxNodeList, NodeList, IsMixamoFbx)) return false;
		}

		return true;
	}

	bool CFBXImporter::ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<FbxNode*>& pFbxNodeList, const std::vector<FbxMesh*>& pFbxMeshList, const std::shared_ptr<animation::CSkeleton>& Skeleton)
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
			Node->SetMeshIndex(MeshIndex);

			// BoneがあるならSkeletonが1つあるとする
			int SkeletonIndex = (Skeleton->GetBoneList().size() > 0)? 0 : - 1;
			Node->SetSkeletonIndex(SkeletonIndex);
		}

		return true;
	}

	bool CFBXImporter::CreateAnimationSkeleton(api::IGraphicsAPI* pGraphicsAPI, FbxNode* pFBXNode, std::shared_ptr<animation::CSkeleton>& Skeleton, std::vector<FbxNode*>& FbxBoneList, const std::vector<std::shared_ptr<object::CNode>>& NodeList, const bool IsMixamoFbx)
	{
		if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			auto BoneNode = GetBoneNode(pFBXNode->GetName(), NodeList);

			std::shared_ptr<animation::CBone> Bone = std::make_shared<animation::CBone>(BoneNode);

			// SkeletonのInverseBindMatrixを作成
			glm::mat4 InverseBindMatrix = glm::inverse(Bone->GetBoneNode()->GetWorldMatrix());
			Bone->GetBoneNode()->SetInverseBindMatrix(InverseBindMatrix);

			// BoneNameを取得
			animation::EHumanoidBones BoneName = pGraphicsAPI->GetBoneNameProvider()->GetBoneName(pFBXNode->GetName());

			// BoneにBoneNameを割り当てる
			Bone->SetBoneName(BoneName);

			Skeleton->AddBone(Bone);

			// FbxBoneListを登録
			FbxBoneList.push_back(pFBXNode);
		}

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			if (!CreateAnimationSkeleton(pGraphicsAPI, pFBXNode->GetChild(i), Skeleton, FbxBoneList, NodeList, IsMixamoFbx)) return false;
		}

		return true;
	}

	void CFBXImporter::ApplyParentBoneList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& Bone : Skeleton->GetBoneList())
		{
			const auto& ParentNode = std::get<1>(Bone)->GetBoneNode()->GetParentNode();
			if (!ParentNode) continue;

			animation::EHumanoidBones ParentBoneName = pGraphicsAPI->GetBoneNameProvider()->GetBoneName(ParentNode->GetName());

			const auto& ParentBone = Skeleton->GetBone(ParentBoneName);
			if (!ParentBone) continue;

			std::get<1>(Bone)->SetParentBoneName(ParentBone->GetBoneName());
		}
	}

	bool CFBXImporter::CreateAnimation(api::IGraphicsAPI* pGraphicsAPI, FbxScene* Scene, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<FbxNode*>& FbxBoneList, const bool IsMixamoFbx)
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

				for (const auto& pFbxBone : FbxBoneList) 
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

					// pFbxBoneListとCSkeletonからアニメーション情報を取得する
					for (int BoneIndex = 0; BoneIndex < FbxBoneList.size(); BoneIndex++)
					{
						const auto& pFbxBone = FbxBoneList[BoneIndex];
						if (!pFbxBone) return false;

						glm::mat4 CurrentMatrix = glm::mat4(1.0f);

						FbxAMatrix fbxMat = pFbxBone->EvaluateLocalTransform(currentTime);
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

							math::CTransform::CalcModelMatrix(CurrentMatrix, Pos, Rotation, false);
						}
						
						// Input
						float InputData = static_cast<float>(currentTime.GetSecondDouble());
						
						// Output
						glm::mat4 OutputMat = CurrentMatrix;
						std::vector<float> OutputData(16);
						std::memcpy(&OutputData[0], &OutputMat[0][0], sizeof(glm::mat4));

						// Create KeyFrame
						// FBX SDKでは全てVALUE_TYPE_MATRIX
						std::shared_ptr<animation::CKeyFrame> KeyFrame = std::make_shared<animation::CKeyFrame>(math::EValueType::VALUE_TYPE_MATRIX);

						KeyFrame->SetInput(InputData);
						KeyFrame->SetOutput(OutputData);

						// Add KeyFrame To Sampler
						if (AnimationSamplerList[BoneIndex]->GetKeyFrameList().size() < static_cast<size_t>(AnimationLength))
						{
							AnimationSamplerList[BoneIndex]->AddKeyFrame(KeyFrame);
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
			for (int BoneIndex = 0; BoneIndex < FbxBoneList.size(); BoneIndex++)
			{
				const auto& pFbxBone = FbxBoneList[BoneIndex];

				// アニメーションのローカル軸を使用するか
				// FBXでは必須でglTF/VRMでは不要
				// 詳しくはCAnimationChannel::UpdateRotationを参照

				// SmallFBXはtrueでFBX SDKはfalse ???(様子を見る)
				const bool UseAnimLocalAxis = false;

				// FBXにはchannelといった概念はなく、Translation・Rotation・Scaleを全てまとめてModelMatrixで計算している
				// なのでChannelTypeにFBX-SDK限定の値としてMODELMATRIXを作成することで対応する
				animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::MODELMATRIX;

				// Boneの順番と参照するSamplerの順番は同じである
				int TargetSamplerIndex = BoneIndex;

				std::string Name = pFbxBone->GetName();

				// アニメーションのターゲットを取得する
				const auto& TargetNode = GetBoneNode(Name, NodeList);

				// Bone Name を取得
				animation::EHumanoidBones BoneName = pGraphicsAPI->GetBoneNameProvider()->GetBoneName(Name);

				std::shared_ptr<animation::CAnimationChannel> AnimationChannel = std::make_shared<animation::CAnimationChannel>(UseAnimLocalAxis, false, TargetSamplerIndex, AnimationTarget, TargetNode->GetName(), BoneName);

				AnimationClip->AddAnimationChannel(AnimationChannel);
			}

			//
			AnimationClip->SetDefaultSkeleton(Skeleton);

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

	std::shared_ptr<object::CNode> CFBXImporter::GetBoneNode(const std::string& BoneName, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		std::shared_ptr<object::CNode> BoneNode = nullptr;

		for (const auto& Node : NodeList)
		{
			if (Node->GetName() == BoneName)
			{
				BoneNode = Node;

				break;
			}
		}

		return BoneNode;
	}

	unsigned int CFBXImporter::FindBoneIndexUsingName(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::string& BoneName)
	{
		unsigned int BoneIndex = 0;

		for (int j = 0; j < Skeleton->GetBoneList().size(); j++)
		{
			const auto& Bone = Skeleton->GetBoneList()[j];

			if (std::get<1>(Bone)->GetBoneNode()->GetName() == BoneName)
			{
				BoneIndex = j;

				break;
			}
		}

		return BoneIndex;
	}

	bool CFBXImporter::CheckIsMixamo(FbxNode* pFBXNode)
	{
		if (std::string(pFBXNode->GetName()).find("mixamo") != -1) return true;

		for (int i = 0; i < pFBXNode->GetChildCount(); i++)
		{
			FbxNode* pChildFBXNode = pFBXNode->GetChild(i);

			if (CheckIsMixamo(pChildFBXNode)) return true;
		}

		return false;
	}
}
#endif // USE_FBX