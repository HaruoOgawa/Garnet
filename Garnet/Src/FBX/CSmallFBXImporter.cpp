#if defined(USE_FBX) && defined(USE_SMALL_FBX)
#include "CSmallFBXImporter.h"

#include "../Animation/CBoneNameProvider.h"

#include "../Object/C3DObject.h"

#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkin.h"
#include "../Animation/CJoint.h"

#include "../Graphics/CMaterialFrame.h"

namespace fbx
{
	bool CSmallFBXImporter::ImportFBX(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;

		if (!Import(pGraphicsAPI, Data, true, Object, AnimationClipList, MaterialFrame)) return false;

		return true;
	}

	bool CSmallFBXImporter::ImportFBXAnimation(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList)
	{
		std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

		if (!Import(pGraphicsAPI, Data, false, Object.get(), AnimationClipList, nullptr)) return false;

		return true;
	}

	bool CSmallFBXImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		std::istringstream stream(std::string(Data.begin(), Data.end()));

		sfbx::DocumentPtr Doc = sfbx::MakeDocument();
		Doc->readBinary(stream);

		if (!Analyse(pGraphicsAPI, Doc, IsUseObject, Object, AnimationClipList, MaterialFrame)) return false;

		return true;
	}

	bool CSmallFBXImporter::Analyse(api::IGraphicsAPI* pGraphicsAPI, const sfbx::DocumentPtr& Doc, bool IsUseObject, object::C3DObject* Object,
		std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;
		std::vector<sfbx::Object*> pFbxNodeList;

		if (!CreateNodeList(Doc, pFbxNodeList, NodeList, RootNodeIndexList)) return false;

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
		std::vector<sfbx::Object*> FbxJointList;

		for (const auto& RootNode : Doc->getRootObjects())
		{
			if (RootNode->getClass() != sfbx::ObjectClass::Model) continue;

			if (RootNode)
			{
				if (!CreateAnimationSkin(RootNode, Skin, FbxJointList, NodeList)) return false;
			}
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
		if (!CreateAnimation(Doc, AnimationClipList, NodeList, Skin, FbxJointList)) return false;

		if (IsUseObject)
		{
			std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
			std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
			std::vector<std::shared_ptr<graphics::CMesh>> MeshList;

			if (Doc->getRootObjects().size() > 0)
			{
				// 描画情報の取得
				std::vector<sfbx::Mesh*> pFbxMeshList;
				for (const auto& RootNode : Doc->getRootObjects())
				{
					if (RootNode->getClass() != sfbx::ObjectClass::Model) continue;

					if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, MaterialFrame, RootNode, TextureList, MaterialList, MeshList, Skin)) return false;
				}

				// マテリアルを持っていないのならダミーを渡す
				if (MaterialList.size() <= 0)
				{
					if (!CreateDummyMaterial(pGraphicsAPI, MaterialList, MaterialFrame, MeshList, Skin)) return false;
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
		}

		return true;
	}

	bool CSmallFBXImporter::CreateNodeList(const sfbx::DocumentPtr& Doc, std::vector<sfbx::Object*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		int RootIndex = 0;

		for (const auto& RootObj : Doc->getRootObjects())
		{
			if (RootObj->getClass() != sfbx::ObjectClass::Model) continue;

			if (!RootObj) continue;

			RootNodeIndexList.push_back(std::vector<int>(1, RootIndex));

			if (!CreateNode(RootObj, pFbxNodeList, NodeList)) return false;

			// 子要素を登録する
			if (pFbxNodeList.size() != NodeList.size()) return false;

			for (int n = 0; n < NodeList.size(); n++)
			{
				sfbx::Object* pFBXNode = pFbxNodeList[n];
				const auto& Node = NodeList[n];

				std::vector<int> ChildNodeList;

				for (int i = 0; i < pFBXNode->getChildren().size(); i++)
				{
					sfbx::Object* pChildNode = pFBXNode->getChild(i);

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

			//
			RootIndex++;
		}

		return true;
	}

	bool CSmallFBXImporter::CreateNode(sfbx::Object* pFBXNode, std::vector<sfbx::Object*>& pFbxNodeList, std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		if (pFBXNode->getClass() != sfbx::ObjectClass::NodeAttribute && pFBXNode->getClass() != sfbx::ObjectClass::Model && pFBXNode->getClass() != sfbx::ObjectClass::Geometry) return true;

		// Nodeを作成
		// MeshとSkinは後ほどセットする
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(-1, std::vector<std::shared_ptr<graphics::CMesh>>(), std::vector<std::shared_ptr<graphics::CMaterial>>());

		std::string NodeName = std::string(pFBXNode->getName());
		Node->SetName(NodeName);

		glm::vec3 Pos = glm::vec3(0.0f);
		glm::quat Rotation = glm::quat(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		glm::vec3 Scale = glm::vec3(1.0f);

		if (auto limbNode = sfbx::as<sfbx::LimbNode>(pFBXNode))
		{
			glm::mat4 LocalMatrix = glm::mat4(1.0f);

			auto fbxMat = limbNode->getLocalMatrix();
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					LocalMatrix[row][col] = static_cast<float>(fbxMat[row][col]);
				}
			}

			math::CTransform::CastModelMatrixToTransform(LocalMatrix, Pos, Rotation, Scale);
		}
		else if (auto limbNodeAttrib = sfbx::as<sfbx::LimbNodeAttribute>(pFBXNode))
		{
			// LimbNodeAttributeは無視する
			return true;
		}
		else
		{
			// 構文は公式サンプルを参照
			// https://github.com/i-saint/WebAlembicViewer/blob/master/src/SceneFBX.cpp#L72
			if (auto model = sfbx::as<sfbx::Model>(pFBXNode))
			{
				sfbx::float3 fbxTranslation = model->getPosition();
				sfbx::float3 fbxRotation = model->getRotation();
				sfbx::float3 fbxScale = model->getScale();

				Pos = glm::vec3(static_cast<float>(fbxTranslation[0]), static_cast<float>(fbxTranslation[1]), static_cast<float>(fbxTranslation[2]));
				Rotation =
					glm::angleAxis(static_cast<float>(fbxRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::angleAxis(static_cast<float>(fbxRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::angleAxis(static_cast<float>(fbxRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
				Scale = glm::vec3(static_cast<float>(fbxScale[0]), static_cast<float>(fbxScale[1]), static_cast<float>(fbxScale[2]));
			}
		}

		// FbxはTranslation・Posが100倍になっているので調整する
		// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
		math::CTransform::CastCentiMeter2Meter(Pos);

		Node->SetPos(Pos);
		Node->SetRot(Rotation);
		Node->SetScale(Scale);

		// ノードを登録
		NodeList.push_back(Node);

		pFbxNodeList.push_back(pFBXNode);

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->getChildren().size(); i++)
		{
			if (!CreateNode(pFBXNode->getChild(i), pFbxNodeList, NodeList)) return false;
		}

		return true;
	}

	bool CSmallFBXImporter::ConnectNodeTo(std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<sfbx::Object*>& pFbxNodeList, const std::vector<sfbx::Mesh*>& pFbxMeshList,
		const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		// NodeListとpFbxNodeListは同じ順番で同じ数
		if (NodeList.size() != pFbxNodeList.size()) return false;

		for (int i = 0; i < NodeList.size(); i++)
		{
			auto& Node = NodeList[i];
			sfbx::Object* pFBXNode = pFbxNodeList[i];

			// Listの要素のポインターとpFBXNodeが持ってる要素のポインターを比較してリスト内の順番をIndexとして渡す
			int MeshIndex = -1;
			int Loop = 0;
			for (auto it = pFbxMeshList.begin(); it != pFbxMeshList.end(); it++)
			{
				if (auto pTargetMesh = sfbx::as<sfbx::Mesh>(*it))
				{
					if (auto pFBXMesh = sfbx::as<sfbx::Mesh>(pFBXNode))
					{
						if (pTargetMesh == pFBXMesh)
						{
							MeshIndex = Loop;

							break;
						}
					}
				}
				

				Loop++;
			}

			// MeshIndexを設定
			Node->SetMeshIndexWithDynamicOffset(MeshIndex, MeshList, MaterialList);

			// JointがあるならSkinが1つあるとする
			int SkinIndex = (Skin && Skin->GetJointList().size() > 0) ? 0 : -1;
			Node->SetSkinIndex(SkinIndex);
		}

		return true;
	}

	bool CSmallFBXImporter::CreateDrawInfo(api::IGraphicsAPI* pGraphicsAPI, std::vector<sfbx::Mesh*>& pFbxMeshList, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame,
		sfbx::Object* pFBXNode, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
		std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
	{

		// 知りたいのは描画情報なのでここではeMeshのみ見る
		//if (pFBXNode->GetNodeAttribute() && pFBXNode->GetNodeAttribute()->GetAttributeType() && pFBXNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		if(auto fbxMesh = sfbx::as<sfbx::Mesh>(pFBXNode))
		{
			// テクスチャ

			// マテリアル

			// メッシュ
			if (!CreateMesh(pFBXNode, pFbxMeshList, MeshList, Skin)) return false;
		}

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->getChildren().size(); i++)
		{
			if (!CreateDrawInfo(pGraphicsAPI, pFbxMeshList, MaterialFrame, pFBXNode->getChild(i), TextureList, MaterialList, MeshList, Skin)) return false;
		}

		return true;
	}

	bool CSmallFBXImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		if (!MaterialFrame) return true;

		// マテリアルにシェーダーを設定
		std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI);

		// SkinMatrix StorageBuffer
		{
			// SkinMatは存在するJointの数だけ用意する必要がある
			int SkinMatCount = 1;
			if (Skin && Skin->GetJointList().size() > 0) SkinMatCount = static_cast<int>(Skin->GetJointList().size());

			std::vector<glm::mat4> SkinMatrixList;
			SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

			material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
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

	bool CSmallFBXImporter::CreateMesh(sfbx::Object* pFBXNode, std::vector<sfbx::Mesh*>& pFbxMeshList, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<animation::CSkin>& Skin)
	{
		if (auto pFbxMesh = sfbx::as<sfbx::Mesh>(pFBXNode))
		{
			auto pFbxGeom = pFbxMesh->getGeometry();
			if (!pFbxGeom) return true;

			// MeshListに登録
			pFbxMeshList.push_back(pFbxMesh);

			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			{
				// 頂点データに使用するJoint・Weightsを取得する
				// https://www.gamedev.net/tutorials/_/technical/graphics-programming-and-theory/how-to-work-with-fbx-sdk-r3582/
				std::vector<std::vector<std::pair<unsigned int, float>>> JointWeightPairPerCtrlPoint(pFbxGeom->getPoints().size());

				if(Skin)
				{
					// 処理中のMeshが関連しているSkinのJointデータを取得する
					unsigned int numOfDeformers = static_cast<unsigned int>(pFbxGeom->getDeformers().size());
					
					// Deformer(Skin)を取得する
					for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; deformerIndex++)
					{
						sfbx::Skin* pFbxSkin = sfbx::as<sfbx::Skin>(pFbxGeom->getDeformers()[deformerIndex]);

						if (!pFbxSkin) continue;

						// Cluster(Joint)を取得
						unsigned int numOfCluster = static_cast<unsigned int>(pFbxSkin->getClusters().size());

						for (unsigned int clusterIndex = 0; clusterIndex < numOfCluster; clusterIndex++)
						{
							sfbx::Cluster* pFbxCluster = pFbxSkin->getClusters()[clusterIndex];
							if (!pFbxCluster) continue;
							
							std::string jointName = std::string(pFbxCluster->getName().begin(), pFbxCluster->getName().end());
							// 余分な文字が入っていたら排除する
							if (jointName.find("Cluster ") != -1) jointName = jointName.substr(8);

							unsigned int JointIndex = FindJointIndexUsingName(Skin, jointName);
							if (JointIndex == -1) continue;

							const auto& Weights = pFbxCluster->getWeights(); // このJointを参照している頂点のWeightリスト
							const auto& VertArrayUsingJoint = pFbxCluster->getIndices(); // このJointを参照している頂点のインデックスリスト

							// コントロールポイント == 頂点
							// このJointを参照している頂点の数
							unsigned int VertNumUsingJoint = static_cast<unsigned int>(VertArrayUsingJoint.size());

							for (unsigned int i = 0; i < VertNumUsingJoint; i++)
							{
								int ControlPointIndex = VertArrayUsingJoint[i];

								std::pair<unsigned int, float> JointWeightPair = { JointIndex , static_cast<float>(Weights[i]) };

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
					const auto& fbxIndices = pFbxGeom->getIndices();
					if (fbxIndices.size() % 4 == 0)
					{
						// 四角形ポリゴンを三角ポリゴンに変換する際に使用するインデックス
						int IndexArray[6] = { 0, 1, 2, 0, 2, 3 };

						for (int i = 0; i < fbxIndices.size(); i += 4)
						{
							for (int s : IndexArray)
							{
								Indices.push_back(static_cast<unsigned short>(fbxIndices[i + s]));
							}
						}
					}
					else
					{
						// 三角形ポリゴン
						for (int Index : fbxIndices)
						{
							Indices.push_back(static_cast<unsigned short>(Index));
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

						const auto& Points = pFbxGeom->getPoints();
						const auto& NormalLayers = pFbxGeom->getNormalLayers();
						const auto& UVLayers = pFbxGeom->getUVLayers();

						// 頂点座標
						if(Points.size() > 0)
						{
							for (const auto& pFbxPosition : Points)
							{
								glm::vec3 Pos = glm::vec3(static_cast<float>(pFbxPosition[0]), static_cast<float>(pFbxPosition[1]), static_cast<float>(pFbxPosition[2]));

								// FbxはTranslation・Posが100倍になっているので調整する
								// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
								math::CTransform::CastCentiMeter2Meter(Pos);

								AttributePosData.push_back(Pos.x);
								AttributePosData.push_back(Pos.y);
								AttributePosData.push_back(Pos.z);
							}
						}
						else
						{
							return false;
						}

						// 法線
						if (NormalLayers.size() > 0)
						{
							
							for (const auto& Normals : NormalLayers)
							{
								for (auto Normal = Normals.data.begin(); Normal != Normals.data.end(); Normal++)
								{
									AttributeNormalData.push_back(static_cast<float>((*Normal).x));
									AttributeNormalData.push_back(static_cast<float>((*Normal).y));
									AttributeNormalData.push_back(static_cast<float>((*Normal).z));
								}
							}
						}

						// UV
						if(UVLayers.size() > 0)
						{
							for (const auto& UVs : UVLayers)
							{
								for (auto UV = UVs.data.begin(); UV != UVs.data.end(); UV++)
								{
									AttributeUVData.push_back(static_cast<float>((*UV).x));
									AttributeUVData.push_back(static_cast<float>((*UV).y));
								}
							}
						}

						// 接線
						{
							// 接線はひとまずインターフェースがないということで後で再計算する
						}

						// Joint・Weights
						for(int CtrlPointIndex = 0; CtrlPointIndex < Points.size(); CtrlPointIndex++)
						{
							// 後回し
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

									// 
									AttributeWeightsData.push_back(0.0f);
								}
							}
						}

						// 頂点座標
						if (!AttributePosData.empty())
						{
							// データを登録
							ReservedVertexDataList.insert({ "POSITION" ,AttributePosData });

							// コンポーネントタイプ(データ型)を取得
							ReservedDataTypeList.insert({ "POSITION", renderer::EDataType::TYPE_FLOAT });

							// ByteStrideを取得
							ReservedByteStrideList.insert({ "POSITION", 0 });
						}

						// 法線
						if (!AttributeNormalData.empty())
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
							int VertexCount = static_cast<int>(pFbxGeom->getPoints().size());

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
						//if (!RecalculateTangent(ReservedVertexDataList["TANGENT"], ReservedVertexDataList["POSITION"], ReservedVertexDataList["TEXCOORD_0"], Indices)) return false;
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

	bool CSmallFBXImporter::CreateAnimationSkin(sfbx::Object* pFBXNode, std::shared_ptr<animation::CSkin>& Skin, std::vector<sfbx::Object*>& FbxJointList, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		if (auto limbNode = sfbx::as<sfbx::LimbNode>(pFBXNode))
		{
			std::string Name = std::string(limbNode->getName().begin(), limbNode->getName().end());

			auto JointNode = GetJointNode(Name, NodeList);
			if (JointNode)
			{
				std::shared_ptr<animation::CJoint> Joint = std::make_shared<animation::CJoint>(JointNode);

				// SkinのInverseBindMatrixを作成
				glm::mat4 InverseBindMatrix = glm::inverse(Joint->GetJointNode()->GetWorldMatrix());
				Joint->GetJointNode()->SetInverseBindMatrix(InverseBindMatrix);

				// BoneNameを取得
				std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
				animation::EHumanoidBones BoneName = Provider->GetBoneName(Name);

				// JointにBoneNameを割り当てる
				Joint->SetBoneName(BoneName);

				Skin->AddJoint(Joint);

				// FbxJointListを登録
				FbxJointList.push_back(pFBXNode);
			}
		}

		// 子要素のNodeを調べる
		for (int i = 0; i < pFBXNode->getChildren().size(); i++)
		{
			if (!CreateAnimationSkin(pFBXNode->getChild(i), Skin, FbxJointList, NodeList)) return false;
		}

		return true;
	}

	void CSmallFBXImporter::ApplyParentJointList(const std::shared_ptr<animation::CSkin>& Skin, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
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

	bool CSmallFBXImporter::CreateAnimation(const sfbx::DocumentPtr& Doc, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::shared_ptr<animation::CSkin>& Skin, const std::vector<sfbx::Object*>& FbxJointList)
	{
		for (int i = 0; i < Doc->getAnimationStacks().size(); i++)
		{
			std::shared_ptr<animation::CAnimationClip> AnimationClip = std::make_shared<animation::CAnimationClip>();
			
			// AnimStackはアニメーションクリップのようなもの
			sfbx::AnimationStack* pAnimStack = Doc->getAnimationStacks()[i];
			std::string animStackName = std::string(pAnimStack->getName().begin(), pAnimStack->getName().end());

			for (const auto& pFbxAnimationLayer : pAnimStack->getAnimationLayers())
			{
				for (int SamplerIndex = 0; SamplerIndex < pFbxAnimationLayer->getAnimationCurveNodes().size(); SamplerIndex++)
				{
					const auto& pFbxCurveNode = pFbxAnimationLayer->getAnimationCurveNodes()[SamplerIndex];

					sfbx::AnimationKind AnimationKind = pFbxCurveNode->getAnimationKind();
					animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::NONE;

					if (AnimationKind == sfbx::AnimationKind::Position)
					{
						AnimationTarget = animation::EAnimationTarget::TRANSLATION;
					}
					else if (AnimationKind == sfbx::AnimationKind::Rotation)
					{
						AnimationTarget = animation::EAnimationTarget::ROTATION;
					}
					else if (AnimationKind == sfbx::AnimationKind::Scale)
					{
						AnimationTarget = animation::EAnimationTarget::SCALE;
					}
					else if (AnimationKind == sfbx::AnimationKind::DeformWeight)
					{
						AnimationTarget = animation::EAnimationTarget::WEIGHTS;
					}

					// samplers
					{
						std::vector<float> inputList;
						std::vector<float> outputList;

						animation::EInterpolationType InterpolationType = animation::EInterpolationType::LINEAR;

						int NumComponent = static_cast<int>(pFbxCurveNode->getAnimationCurves().size());
						animation::EKeyFrameType KeyFrameType = animation::EKeyFrameType::KEYFRAME_TYPE_NONE;

						// ROTATIONの時はQuaternionに変換する
						if (AnimationTarget == animation::EAnimationTarget::ROTATION)
						{
							NumComponent = 4;
						}

						// SmallFbxはVec3かScalerしか存在しない
						if (NumComponent == 3)
						{
							KeyFrameType = animation::EKeyFrameType::KEYFRAME_TYPE_VEC3;
						}
						else if (NumComponent == 1)
						{
							KeyFrameType = animation::EKeyFrameType::KEYFRAME_TYPE_SCALAR;
						}
						else if (NumComponent == 4)
						{
							KeyFrameType = animation::EKeyFrameType::KEYFRAME_TYPE_VEC4;
						}
						else if (NumComponent == 0)
						{
							// 0個は無効な値
							return false;
						}
							
						if (pFbxCurveNode->getAnimationCurves().size() == 0) continue;

						const auto& Times = pFbxCurveNode->getAnimationCurves()[0]->getTimes();
						inputList.resize(Times.size());
						std::memcpy(&inputList[0], &Times[0], sizeof(float) * Times.size());

						std::vector<std::vector<float>> ValuesList;

						for (const auto& pFbxCurve : pFbxCurveNode->getAnimationCurves())
						{
							std::vector<float> DstValues;

							const auto& RawValues = pFbxCurve->getRawValues();
							DstValues.resize(RawValues.size());
							std::memcpy(&DstValues[0], &RawValues[0], sizeof(float) * RawValues.size());

							ValuesList.push_back(DstValues);
						}

						// OutputDataを構築
						if (ValuesList.size() == 0) return false;
						int size = static_cast<int>(ValuesList[0].size());

						for (const auto& Values : ValuesList)
						{
							if (size != Values.size()) return false;
						}

						// ROTATIONの時はQuaternionに変換する
						if (AnimationTarget == animation::EAnimationTarget::ROTATION)
						{
							if (ValuesList.size() != 3) return false;

							for (int v = 0; v < ValuesList[0].size(); v++)
							{
								glm::quat quat = 
									glm::angleAxis(ValuesList[2][v], glm::vec3(0.0f, 0.0f, 1.0f)) * 
									glm::angleAxis(ValuesList[1][v], glm::vec3(0.0f, 1.0f, 0.0f)) * 
									glm::angleAxis(ValuesList[0][v], glm::vec3(1.0f, 0.0f, 0.0f));

								outputList.push_back(quat.x);
								outputList.push_back(quat.y);
								outputList.push_back(quat.z);
								outputList.push_back(quat.w);
							}
						}
						else if(AnimationTarget == animation::EAnimationTarget::TRANSLATION)
						{
							if (ValuesList.size() != 3) return false;

							for (int v = 0; v < ValuesList[0].size(); v++)
							{
								// FbxはTranslation・Posが100倍になっているので調整する
								// たぶん単位がcmなので0.01倍することで計算に一般的に使用するmに直す
								glm::vec3 Pos = glm::vec3(ValuesList[0][v], ValuesList[1][v], ValuesList[2][v]);
								math::CTransform::CastCentiMeter2Meter(Pos);

								outputList.push_back(Pos.x);
								outputList.push_back(Pos.y);
								outputList.push_back(Pos.z);
							}
						}
						else 
						{
							for (int v = 0; v < ValuesList[0].size(); v++)
							{
								for (const auto& Values : ValuesList)
								{
									outputList.push_back(Values[v]);
								}
							}
						}

						std::shared_ptr<animation::CAnimationSampler> AnimationSampler = std::make_shared<animation::CAnimationSampler>(InterpolationType);
						if (!AnimationSampler->CreateKeyFrame(KeyFrameType, inputList, outputList)) return false;

						AnimationClip->AddAnimationSampler(AnimationSampler);
					}

					// channels
					{
						const auto& pFbxAnimTarget = pFbxCurveNode->getAnimationTarget();
						
						// SamplerをClipに追加する順番とChannelを追加する順番は同じである
						int TargetSamplerIndex = SamplerIndex;

						std::string JointName = std::string(pFbxAnimTarget->getName().begin(), pFbxAnimTarget->getName().end());

						// アニメーションのターゲットを取得する
						const auto& TargetNode = GetJointNode(JointName, NodeList);

						// Bone Name を取得
						std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
						animation::EHumanoidBones BoneName = Provider->GetBoneName(JointName);

						std::shared_ptr<animation::CAnimationChannel> AnimationChannel = std::make_shared<animation::CAnimationChannel>(TargetSamplerIndex, AnimationTarget, TargetNode, BoneName);

						AnimationClip->AddAnimationChannel(AnimationChannel);
					}
				}
			}

			AnimationClip->SetDefaultSkin(Skin);

			AnimationClipList.push_back(AnimationClip);
		}

		return true;
	}

	// Helper Function //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CSmallFBXImporter::RecalculateTangent(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices)
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

	std::shared_ptr<object::CNode> CSmallFBXImporter::GetJointNode(const std::string& JointName, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
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

	unsigned int CSmallFBXImporter::FindJointIndexUsingName(const std::shared_ptr<animation::CSkin>& Skin, const std::string& JointName)
	{
		unsigned int JointIndex = -1;

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
#endif