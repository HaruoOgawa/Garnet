#ifdef USE_MMD
#include "CPmxImporter.h"
#include "../../Message/Console.h"
#include "../../Math/CMath.h"
#include "../../Math/CTransform.h"
#include "../../Object/C3DObject.h"
#include "../../Animation/CAnimationClip.h"
#include "../../Animation/CSkeleton.h"
#include "../../Animation/CBone.h"
#include "../../Animation/CBoneNameProvider.h"

#include "../../Graphics/CMaterialFrame.h"
#include "../../Graphics/CVertexBuffer.h"
#include "../../Graphics/CIndexBuffer.h"

#include "../../LoadWorker/CLoadWorker.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../LoadWorker/CTextureLoader.h"

namespace mmd
{
	bool CPmxImporter::ImportPmx(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::string& ModelFileName, 
		const std::vector<unsigned char>& Data, object::C3DObject* Object, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader, 
		animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList)
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
		std::vector<int> RootNodeIndexList;

		// ルートノードを作成
		std::shared_ptr<object::CNode> RootNode = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));
		RootNode->SetName("RootNode");
		RootNode->SetU16Name(L"RootNode");
		NodeList.push_back(RootNode);

		RootNodeIndexList.push_back(0);

		// Skeleton
		std::shared_ptr<animation::CSkeleton> Skeleton = std::make_shared<animation::CSkeleton>(RigType, (Object->GetObjectName() + "_Skeleton"));
		if (!CreateAnimationSkeleton(model, Skeleton, NodeList, RootNode)) return false;

		// BoneTableを作成
		Skeleton->MakeHumanoidBoneTable(HumanoidBoneList);

		// IKBoneListを作成
		Skeleton->MakeIKBoneList();

		// 付与ボーンリストを作成
		Skeleton->MakeGrantBoneList();

		// マテリアルリスト
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		std::map<int, std::vector<std::shared_ptr<graphics::CMaterial>>> SharedToonUsingList;
		if (!CreateMaterialList(pGraphicsAPI, model, MaterialList, SharedToonUsingList, MaterialFrame, Skeleton)) return false;

		// テクスチャリスト
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;

		if (!CreateTextureList(pGraphicsAPI, p3DObjectLoader, ModelFileName, model, TextureList, SharedToonUsingList)) return false;

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMeshList(pGraphicsAPI, Object, model, MeshList, RootNode, NodeList, MaterialList, (Skeleton->GetBoneList().size() > 0))) return false;

		// 剛体
		std::vector<std::shared_ptr<physics::IPhysicsObject>> PhysicsObjectList;
		if (!CreateRigidbody(pPhysicsEngine, model, Skeleton, PhysicsObjectList)) return false;

		// ジョイント
		if (!CreateJoint(pPhysicsEngine, model, Skeleton, PhysicsObjectList)) return false;

		// リソースを登録
		Object->SetRootNodeIndexList(RootNodeIndexList);

		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		Object->SetAnimationSkeleton(Skeleton);

		for (const auto& Material : MaterialList)
		{
			Object->AddMaterial(Material);
		}

		for (const auto& Texture : TextureList)
		{
			Object->GetTextureSet()->Add2DTexture(Texture);
		}

		for (const auto& Mesh : MeshList)
		{
			Object->AddMesh(Mesh);
		}

		// DefaultMatrixを保存
		Object->ApplyDefaultLocalTransform();

		// WorldMatrixを計算
		Object->CalcWorldMatrix();

		// ParentNodeを設定する
		Object->ApplyParentNode();

		// 逆バインドポーズを計算する
		if (!CalcInverseBindPose(Skeleton)) return false;

		return true;
	}

	bool CPmxImporter::CreateAnimationSkeleton(const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::shared_ptr<object::CNode>& RootNode)
	{
		animation::CBoneNameProvider Provider;

		// PmxではBoneとBoneは全くの別物でそれぞれ違う役割を持っているので厳格に名前分けする必要がある!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		const auto& PmxBoneList = model.GetPmxBoneList();

		for (int BoneIndex = 0; BoneIndex < PmxBoneList.size(); BoneIndex++)
		{
			const auto& PmxBone = PmxBoneList[BoneIndex];

			// BoneNodeの作成
			std::shared_ptr<object::CNode> BoneNode = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));

			const auto& Name = PmxBone->GetBoneName().second;
			BoneNode->SetU16Name(Name);
			BoneNode->SetName("BoneNode_" + std::to_string(BoneIndex));

			glm::vec3 Pos = PmxBone->GetPos();
			glm::quat Rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			//glm::quat Rot = PmxBone->GetLocalAxis();

			// PMXのPos・Rotateはワールド座標系での値なので親ノードのワールドマトリックスを乗算してローカル座標系に戻す必要がある
			int ParentBoneIndex = PmxBone->GetParentBoneIndex();
			if(ParentBoneIndex >= 0 && ParentBoneIndex < PmxBoneList.size())
			{
				const auto& ParentPmxBone = PmxBoneList[ParentBoneIndex];
				
				// Posはワールド座標系なのでローカル座標系に戻す必要がある
				// ただしRotは(存在すれば)ローカル軸から取得するので既にローカル座標系である
				Pos -= ParentPmxBone->GetPos();
			}

			//
			BoneNode->SetPos(Pos);
			BoneNode->SetRot(Rot);

			BoneNode->SaveAsDefaultLocalTransform();

			NodeList.push_back(BoneNode);

			// Boneを作成
			std::shared_ptr<animation::CBone> Bone = std::make_shared<animation::CBone>(BoneNode);

			// BoneにBoneNameを割り当てる
			animation::EHumanoidBones BoneName = Provider.GetBoneNameU16(Name);
			Bone->SetBoneName(BoneName);

			// ボーンの付与
			if (PmxBone->IsRotateGrant())
			{
				// 回転付与
				Bone->SetRotateGrant(PmxBone->GetGrantParentBoneIndex(), PmxBone->GetGrantRate());
			}
			else if (PmxBone->IsMoveGrant())
			{
				// 移動付与
				Bone->SetMoveGrant(PmxBone->GetGrantParentBoneIndex(), PmxBone->GetGrantRate());
			}

			// IK
			Bone->SetIKParam(PmxBone->GetIKParam());

			Skeleton->AddBone(Bone);
		}

		// BoneNodeに子要素を設定する
		{
			const auto& BoneList = Skeleton->GetBoneList();

			for (int BoneIndex = 0; BoneIndex < PmxBoneList.size(); BoneIndex++)
			{
				const auto& PmxBone = PmxBoneList[BoneIndex];

				const auto& Bone = BoneList[BoneIndex];
				int SelfNodeIndex = std::get<1>(Bone)->GetBoneNode()->GetSelfNodeIndex();

				int ParentBoneIndex = PmxBone->GetParentBoneIndex();

				/// 範囲外を示すときはRootNodeを親に持つ
				if (ParentBoneIndex < 0 || ParentBoneIndex >= BoneList.size())
				{
					RootNode->AddChildrenNodeIndex(SelfNodeIndex);
				}
				else
				{
					// 自身を親ノードの子要素リストに追加する
					std::get<1>(BoneList[ParentBoneIndex])->GetBoneNode()->AddChildrenNodeIndex(SelfNodeIndex);
				}
			}
		}

		return true;
	}

	bool CPmxImporter::CalcInverseBindPose(std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		for (const auto& BonePair : Skeleton->GetBoneList())
		{
			const auto& Bone = std::get<1>(BonePair);

			// MMDのBoneはローカル座標系ではなくワールド座標系なのでセンターとかの親ボーンを考慮するかは迷うところ
			glm::mat4 InverseBindMatrix = glm::inverse(Bone->GetBoneNode()->GetWorldMatrix());
			Bone->GetBoneNode()->SetInverseBindMatrix(InverseBindMatrix);
		}

		return true;
	}

	bool CPmxImporter::CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		std::map<int, std::vector<std::shared_ptr<graphics::CMaterial>>>& SharedToonUsingList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkeleton>& Skeleton)
	{
		if (!MaterialFrame) return false;

		const auto& PmxMaterilList = model.GetPmxMaterialList();

		for (const auto& PmxMaterial : PmxMaterilList)
		{
			// アウトラインにも使用するので2つ参照する
			int MatRefCount = 2;

			graphics::ECullMode CullMode = (PmxMaterial->IsDrawDoubleSlided()) ? graphics::ECullMode::CULL_NONE : graphics::ECullMode::CULL_BACK;

			// マテリアルにシェーダーを設定
			std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI, MatRefCount, CullMode);

			material->ReplacePreloadUniformValue("edgeSize", &glm::vec1(PmxMaterial->GetEdgeSize())[0], sizeof(float), 0);

			material->ReplacePreloadUniformValue("baseColorFactor", &PmxMaterial->GetDiffuse()[0], sizeof(glm::vec4), 2);
			material->ReplacePreloadUniformValue("ambientFactor", &PmxMaterial->GetAmbient()[0], sizeof(glm::vec4), 2);
			material->ReplacePreloadUniformValue("specularFactor", &PmxMaterial->GetSpecular()[0], sizeof(glm::vec4), 2);
			material->ReplacePreloadUniformValue("edgeColor", &PmxMaterial->GetEdgeColor()[0], sizeof(glm::vec4), 2);
			material->ReplacePreloadUniformValue("specularIntensity", &glm::vec1(PmxMaterial->GetSpecularCoef())[0], sizeof(float), 2);

			// アウトライン
			material->SetIsDrawOutline(PmxMaterial->IsDrawEdge());

			// アルファブレンド(ひとまず全部で有効にする
			material->SetBlendType(graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA);

			// MainTexture
			{
				int TextureIndex = PmxMaterial->GetMainTexIndex();

				if (TextureIndex != -1)
				{
					material->ReplacePreloadUniformValue("UseMainTexture", &glm::ivec1(1)[0], sizeof(int), 2);
					material->ReplaceTextureIndex("MainTexture", TextureIndex);
				}
			}

			// ToonTexture
			{
				int ToonTexIndex = PmxMaterial->GetToonTexIndex();
				int SharedToonTexIndex = PmxMaterial->GetSharedToonTexIndex();

				if (ToonTexIndex >= 0 && ToonTexIndex < model.GetPmxTextureList().size())
				{
					material->ReplacePreloadUniformValue("UseToonTexture", &glm::ivec1(1)[0], sizeof(int), 2);
					material->ReplaceTextureIndex("ToonTexture", ToonTexIndex);
				}
				else if (SharedToonTexIndex >= 0 && SharedToonTexIndex < model.GetPmxTextureList().size())
				{
					// 共通トゥーンテクスチャ使用リストに追加
					if (SharedToonUsingList.find(SharedToonTexIndex) == SharedToonUsingList.end()) SharedToonUsingList.emplace(SharedToonTexIndex, std::vector<std::shared_ptr<graphics::CMaterial>>());

					SharedToonUsingList[SharedToonTexIndex].push_back(material);
				}
			}

			// SphereTexture
			{
				int SphereTexIndex = PmxMaterial->GetSphereTexIndex();
				EPmxSphereMode SphereMode = PmxMaterial->GetSphereMode();

				if (SphereTexIndex >= 0 && SphereTexIndex < model.GetPmxTextureList().size())
				{
					material->ReplacePreloadUniformValue("UseSphereTexture", &glm::ivec1(1)[0], sizeof(int), 2);
					material->ReplacePreloadUniformValue("SphereMode", &glm::ivec1(static_cast<int>(SphereMode))[0], sizeof(int), 2);

					material->ReplaceTextureIndex("SphereTexture", SphereTexIndex);
				}
			}

			// SkinMatrix StorageBuffer
			{
				// SkinMatは存在するBoneの数だけ用意する必要がある
				// DynamicOffsetが256バイトからしか使えない都合上SkinMatCountの最小値は4とする(4 * 16 * 4 = 256)
				int SkinMatCount = 0;
				if (Skeleton && Skeleton->GetBoneList().size() > 0) SkinMatCount = static_cast<int>(Skeleton->GetBoneList().size());

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

			MaterialList.push_back(material);
		}

		return true;
	}

	bool CPmxImporter::CreateMeshList(api::IGraphicsAPI* pGraphicsAPI, object::C3DObject* Object, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, 
		const std::shared_ptr<object::CNode>& RootNode, std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, bool ExistSkeleton)
	{
		// 明示的にMeshNodeを作成
		std::shared_ptr<object::CNode> MeshNode = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));
		MeshNode->SetName("BaseMeshNode");
		NodeList.push_back(MeshNode);

		// RootNodeにMeshNodeを子要素として登録する
		std::vector<int> ChildrenNodeIndexList = RootNode->GetChildrenNodeIndexList();
		ChildrenNodeIndexList.push_back(static_cast<int>(NodeList.size()) - 1);

		RootNode->SetChildrenNodeIndexList(ChildrenNodeIndexList);

		{
			// メッシュを作成する
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			// 頂点バッファを作成する
			auto VertexBuffer = pGraphicsAPI->CreateVertexBuffer();

			// 頂点バッファ本体
			std::vector<std::vector<float>> VertexDataList;
			std::vector<int> DimentionList;
			std::vector<graphics::EDataType> DataTypeList;
			std::vector<int> ByteStrideList;

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

			// メタデータ
			const auto& MetaData = model.GetMetaData();
			const auto& PmxMesh = model.GetPmxMesh();

			// 頂点バッファを読む
			{
				{
					ReservedVertexDataList.emplace("POSITION", PmxMesh->GetPositionAttribute());
					ReservedDataTypeList.emplace("POSITION", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("POSITION", 0);
				}

				{
					ReservedVertexDataList.emplace("NORMAL", PmxMesh->GetNormalAttribute());
					ReservedDataTypeList.emplace("NORMAL", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("NORMAL", 0);
				}

				{
					ReservedVertexDataList.emplace("TEXCOORD_0", PmxMesh->GetUVAttribute());
					ReservedDataTypeList.emplace("TEXCOORD_0", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TEXCOORD_0", 0);
				}

				{
					std::vector<float> AttributeData;
					AttributeData.resize(4 * (PmxMesh->GetPositionAttribute().size() / 3), 0.0f);

					ReservedVertexDataList.emplace("TANGENT", AttributeData);
					ReservedDataTypeList.emplace("TANGENT", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TANGENT", 0);
				}

				{
					std::vector<float> AttributeData;

					if (MetaData.BoneIndexSize == 1)
					{
						const auto& ByteBoneAttribute = PmxMesh->GetByteBoneAttribute();

						if (!ByteBoneAttribute.empty())
						{
							AttributeData.resize(ByteBoneAttribute.size() / 4);
							std::memcpy(&AttributeData[0], &ByteBoneAttribute[0], sizeof(unsigned char) * ByteBoneAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", graphics::EDataType::TYPE_UNSIGNED_BYTE);
						ReservedByteStrideList.emplace("JOINTS_0", 1 * 4);
					}
					else if (MetaData.BoneIndexSize == 2)
					{
						const auto& UShortBoneAttribute = PmxMesh->GetUShortBoneAttribute();

						if (!UShortBoneAttribute.empty())
						{
							AttributeData.resize(UShortBoneAttribute.size() / 2);
							std::memcpy(&AttributeData[0], &UShortBoneAttribute[0], sizeof(unsigned short) * UShortBoneAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", graphics::EDataType::TYPE_UNSIGNED_SHORT);
						ReservedByteStrideList.emplace("JOINTS_0", 2 * 4);
					}
					else if (MetaData.BoneIndexSize == 4)
					{
						const auto& IntBoneAttribute = PmxMesh->GetUIntBoneAttribute();

						if (!IntBoneAttribute.empty())
						{
							AttributeData.resize(IntBoneAttribute.size());
							std::memcpy(&AttributeData[0], &IntBoneAttribute[0], sizeof(unsigned int) * IntBoneAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", graphics::EDataType::TYPE_UNSIGNED_INT);
						ReservedByteStrideList.emplace("JOINTS_0", 4 * 4);
					}

					// 空の時は0埋めする
					if (AttributeData.empty())
					{
						AttributeData.resize(static_cast<int>(PmxMesh->GetPositionAttribute().size()) / 3 * 4);
					}

					ReservedVertexDataList.emplace("JOINTS_0", AttributeData);

				}

				{
					ReservedVertexDataList.emplace("WEIGHTS_0", PmxMesh->GetWeightAttribute());
					ReservedDataTypeList.emplace("WEIGHTS_0", graphics::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("WEIGHTS_0", 0);
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
			}

			// メッシュ情報を渡す
			VertexBuffer->SetVertices(VertexDataList);
			VertexBuffer->SetAttributeDimensions(DimentionList);
			VertexBuffer->SetAttribDataTypes(DataTypeList);
			VertexBuffer->SetAttribByteStrides(ByteStrideList);

			Mesh->AddVertexBuffer(VertexBuffer);

			const auto& PmxMaterialList = model.GetPmxMaterialList();

			int MatRefOffset = 0;

			for (int MaterialIndex = 0; MaterialIndex < PmxMaterialList.size(); MaterialIndex++)
			{
				auto IndexBuffer = pGraphicsAPI->CreateIndexBuffer();

				const auto& PmxMaterial = PmxMaterialList[MaterialIndex];
				// インデックスバッファを読む
				{
					if (MetaData.VertexIndexSize == 1)
					{
						// 未対応
						Console::Log("[Error] InValid Indices Type - Byte\n");

						return false;
					}
					else if (MetaData.VertexIndexSize == 2)
					{
						// Indicesを取得
						const auto& PmxIndices = PmxMesh->GetUShortIndices();

						// 参照オフセット分ずらして参照カウントの分だけ取得する
						std::vector<unsigned short> Indices;
						Indices.resize(PmxMaterial->GetMatRefIndiceCount());

						std::memcpy(&Indices[0], &PmxIndices[MatRefOffset], sizeof(unsigned short) * PmxMaterial->GetMatRefIndiceCount());

						// Index数が奇数の時はWebGPUでエラーが出るので最後の三角形をもう一度繰り返す
						int IndiceSize = static_cast<int>(Indices.size());
						if (IndiceSize % 2 != 0)
						{
							int Index0 = Indices[IndiceSize - 3];
							int Index1 = Indices[IndiceSize - 2];
							int Index2 = Indices[IndiceSize - 1];

							Indices.push_back(Index0);
							Indices.push_back(Index1);
							Indices.push_back(Index2);
						}

						// Indicesを登録
						IndexBuffer->SetIndices(Indices);
					}
					else if (MetaData.VertexIndexSize == 4)
					{
						// Indicesを取得
						const auto& PmxIndices = PmxMesh->GetUIntIndices();

						// 参照オフセット分ずらして参照カウントの分だけ取得する
						std::vector<unsigned int> UINTIndices;
						UINTIndices.resize(PmxMaterial->GetMatRefIndiceCount());

						std::memcpy(&UINTIndices[0], &PmxIndices[MatRefOffset], sizeof(unsigned int) * PmxMaterial->GetMatRefIndiceCount());

						// Index数が奇数の時はWebGPUでエラーが出るので最後の三角形をもう一度繰り返す
						int IndiceSize = static_cast<int>(UINTIndices.size());
						if (IndiceSize % 2 != 0)
						{
							int Index0 = UINTIndices[IndiceSize - 3];
							int Index1 = UINTIndices[IndiceSize - 2];
							int Index2 = UINTIndices[IndiceSize - 1];

							UINTIndices.push_back(Index0);
							UINTIndices.push_back(Index1);
							UINTIndices.push_back(Index2);
						}

						// Indicesを登録
						IndexBuffer->SetUINTIndices(UINTIndices);
					}

					Mesh->AddIndexBuffer(IndexBuffer);

					// 参照オフセットを更新する
					MatRefOffset += PmxMaterial->GetMatRefIndiceCount();
				}

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer, MaterialIndex);
				
				Mesh->AddPrimitive(Primitive);
			}

			// プリミティブが存在するならメッシュインデックスやモーフなどを登録する
			if (Mesh->GetPrimitiveList().size() > 0)
			{
				// メッシュに渡すモーフデータを準備
				std::vector<std::map<int, glm::vec3>> MorphDataList;
				{
					const auto& PmxMorphList = model.GetPmxVertexMorphList();

					for (int MorphIndex = 0; MorphIndex < static_cast<int>(animation::EBlendShapeName::Max); MorphIndex++)
					{
						animation::EBlendShapeName CurrentShapeName = static_cast<animation::EBlendShapeName>(MorphIndex);
						auto PmxMorph = PmxMorphList.find(CurrentShapeName);

						// 頂点モーフのデータを取得する
						if (PmxMorph != PmxMorphList.end())
						{
							MorphDataList.push_back((*PmxMorph).second->GetVertexMorphList());
						}
						else
						{
							MorphDataList.push_back(std::map<int, glm::vec3>());
						}
					}
				}

				// Pmxでは頂点バッファは１つでインデックスバッファが複数個あり、頂点バッファは全体で共有なので最初のプリミティブを指定する
				// 共有頂点バッファを更新すれば全体のメッシュにモーフが適応できるため
				Mesh->SetMorphDataList(Mesh->GetPrimitiveList()[0], 0, MorphDataList);

				// Pmxは特殊なので他のプリミティブは明示的にモーフを持っているということにする
				for (const auto& Primitive : Mesh->GetPrimitiveList())
				{
					Primitive->SetUseMorph(true);
				}

				// メッシュを登録
				MeshList.push_back(Mesh);

				// ノードに情報を登録
				int SkeletonIndex = (ExistSkeleton) ? 0 : -1;
				MeshNode->SetSkeletonIndex(SkeletonIndex);
				MeshNode->SetMeshIndex(0);

				// モーフノード(ブレンドシェイプノード)として登録
				Object->AddBlendShapeNode(MeshNode);
			}
		}

		return true;
	}

	bool CPmxImporter::CreateTextureList(api::IGraphicsAPI* pGraphicsAPI, resource::C3DObjectLoader* p3DObjectLoader, const std::string& ModelFileName, const CPmxModel& model,
		std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::map<int, std::vector<std::shared_ptr<graphics::CMaterial>>>& SharedToonUsingList)
	{
		for (const auto& PmxTexture : model.GetPmxTextureList())
		{
			//
			const auto& FilePath = PmxTexture->GetFilePath();
			const auto& MetaData = model.GetMetaData();
			std::string ParentDir = resource::CFile::AddPunct(resource::CFile::GetParentDir(ModelFileName));

			//
			std::shared_ptr<graphics::CTexture> Texture = pGraphicsAPI->CreateTexture();

			//
			std::shared_ptr<resource::CTextureLoader> TexLoader = nullptr;

			if (MetaData.EncodeType == EPmxEncodeType::UTF8)
			{
				std::string FullPath = ParentDir + FilePath.first;

				TexLoader = std::make_shared<resource::CTextureLoader>(pGraphicsAPI, FullPath, Texture);
			}
			else if (MetaData.EncodeType == EPmxEncodeType::UTF16)
			{
				std::string FullPath = ParentDir + resource::CFile::CastU16ToU8Str(FilePath.second);

				TexLoader = std::make_shared<resource::CTextureLoader>(pGraphicsAPI, FullPath, Texture);
			}

			//
			TextureList.push_back(Texture);
			p3DObjectLoader->AddSubResource(TexLoader);
		}

		// 共通トゥーンテクスチャを使っていれば登録して有効化する
		for (auto& SharedToonPair : SharedToonUsingList)
		{
			int SharedToonIndex = SharedToonPair.first + 1;

			// テクスチャオブジェクトを生成
			std::shared_ptr<graphics::CTexture> Texture = pGraphicsAPI->CreateTexture();
			std::shared_ptr<resource::CTextureLoader> TexLoader = nullptr;

			std::string NumberStr = std::to_string(SharedToonIndex);
			if (NumberStr.length() < 2) NumberStr = "0" + NumberStr;

			std::string FullPath = "Resources/Textures/SharedToon/toon" + NumberStr + ".bmp";

			TexLoader = std::make_shared<resource::CTextureLoader>(pGraphicsAPI, FullPath, Texture);

			// ロードワーカーに追加する
			TextureList.push_back(Texture);
			p3DObjectLoader->AddSubResource(TexLoader);

			// 各マテリアルのインデックスを設定する
			int MatTexIndex = static_cast<int>(TextureList.size()) - 1;

			for (auto& material : SharedToonPair.second)
			{
				material->ReplacePreloadUniformValue("UseToonTexture", &glm::ivec1(1)[0], sizeof(int), 2);
				material->ReplaceTextureIndex("ToonTexture", MatTexIndex);
			}
		}

		return true;
	}

	bool CPmxImporter::CreateRigidbody(physics::IPhysicsEngine* pPhysicsEngine, const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, 
		std::vector<std::shared_ptr<physics::IPhysicsObject>>& PhysicsObjectList)
	{
		const auto& BoneList = Skeleton->GetBoneList();

		for (const auto& PmxRigidbody : model.GetPmxRigidbodyList())
		{
			// 物理オブジェクトを作成
			std::shared_ptr<physics::IPhysicsObject> PhysicsObject = nullptr;

#ifdef __EMSCRIPTEN__
			// EmscriptenかVSかでなぜか反転する必要があったりなかったりするので調整する(VSとEmscriptenの数値をそろえるため)
			glm::vec3 Pos = CovertToZYX(PmxRigidbody.Pos);
			glm::vec3 Rotate = CovertToZYX(PmxRigidbody.Rotate);
			glm::vec3 Size = PmxRigidbody.Size;
#else
			glm::vec3 Pos = PmxRigidbody.Pos;
			glm::vec3 Rotate = PmxRigidbody.Rotate;
			glm::vec3 Size = CovertToZYX(PmxRigidbody.Size);
#endif

			if (PmxRigidbody.PhysicsShape == EPmxPhysicsShape::SPHERE)
			{
				PhysicsObject = pPhysicsEngine->CreatePhysicsSphere(Size.x, (PmxRigidbody.PhysicsType == EPmxPhysicsType::STATIC), PmxRigidbody.Mass, { PmxRigidbody.RigidbodyName, static_cast<physics::EPhysicsType>(PmxRigidbody.PhysicsType), Pos, Rotate, true, PmxRigidbody.group, PmxRigidbody.NoneCollideGroupFlag, PmxRigidbody.TransDamping, PmxRigidbody.RotateDamping, PmxRigidbody.Repulsion, PmxRigidbody.Friction});
			}
			else if (PmxRigidbody.PhysicsShape == EPmxPhysicsShape::BOX)
			{
				PhysicsObject = pPhysicsEngine->CreatePhysicsBox(Size, (PmxRigidbody.PhysicsType == EPmxPhysicsType::STATIC), PmxRigidbody.Mass, {PmxRigidbody.RigidbodyName, static_cast<physics::EPhysicsType>(PmxRigidbody.PhysicsType), Pos, Rotate, true, PmxRigidbody.group, PmxRigidbody.NoneCollideGroupFlag, PmxRigidbody.TransDamping, PmxRigidbody.RotateDamping, PmxRigidbody.Repulsion, PmxRigidbody.Friction});
			}
			else if (PmxRigidbody.PhysicsShape == EPmxPhysicsShape::CAPSULE)
			{
				PhysicsObject = pPhysicsEngine->CreatePhysicsCapsule(Size.x, Size.y, (PmxRigidbody.PhysicsType == EPmxPhysicsType::STATIC), PmxRigidbody.Mass, { PmxRigidbody.RigidbodyName, static_cast<physics::EPhysicsType>(PmxRigidbody.PhysicsType), Pos, Rotate, true, PmxRigidbody.group, PmxRigidbody.NoneCollideGroupFlag, PmxRigidbody.TransDamping, PmxRigidbody.RotateDamping, PmxRigidbody.Repulsion, PmxRigidbody.Friction });
			}
			else
			{
				continue;
			}

			// 物理オブジェクトを割り当てる
			if (PmxRigidbody.RelationBoneIndex < 0 || PmxRigidbody.RelationBoneIndex >= BoneList.size()) continue;

			std::get<1>(BoneList[PmxRigidbody.RelationBoneIndex])->GetBoneNode()->AddPhysicsObject(PhysicsObject);

			//
			PhysicsObjectList.push_back(PhysicsObject);
		}

		return true;
	}

	bool CPmxImporter::CreateJoint(physics::IPhysicsEngine* pPhysicsEngine, const CPmxModel& model, std::shared_ptr<animation::CSkeleton>& Skeleton, 
		const std::vector<std::shared_ptr<physics::IPhysicsObject>>& PhysicsObjectList)
	{
		const auto& PmxRigidbodyList = model.GetPmxRigidbodyList();

		for (const auto& PmxJoint : model.GetPmxJointList())
		{
			// PhysicsObjectを取得
			// BodyA
			int BodyAIndex = PmxJoint.BodyAIndex;
			if (BodyAIndex < 0 || BodyAIndex >= PmxRigidbodyList.size()) continue;

			const auto& PhysicsObjA = PhysicsObjectList[BodyAIndex];

			// BodyB
			int BodyBIndex = PmxJoint.BodyBIndex;
			if (BodyBIndex < 0 || BodyBIndex >= PmxRigidbodyList.size()) continue;

			const auto& PhysicsObjB = PhysicsObjectList[BodyBIndex];

			if (!PhysicsObjA || !PhysicsObjB) continue;

			// Constraintを予約する
#ifdef __EMSCRIPTEN__
			// EmscriptenかVSかでなぜか反転する必要があったりなかったりするので調整する(VSとEmscriptenの数値をそろえるため)
			PhysicsObjB->ReserveConstraint(PhysicsObjA, static_cast<physics::EJointType>(PmxJoint.PmxJointType),
				{
					PmxJoint.JointName, PmxJoint.Pos, CovertToZYX(PmxJoint.Rotate), CovertToZYX(PmxJoint.LowerTransLimit), CovertToZYX(PmxJoint.UpperTransLimit), 
					CovertToZYX(PmxJoint.LowerRotateLimit), CovertToZYX(PmxJoint.UpperRotateLimit), CovertToZYX(PmxJoint.TransSpring), CovertToZYX(PmxJoint.RotateSpring)
				}
			);
#else
			PhysicsObjB->ReserveConstraint(PhysicsObjA, static_cast<physics::EJointType>(PmxJoint.PmxJointType),
				{
					PmxJoint.JointName, CovertToZYX(PmxJoint.Pos), PmxJoint.Rotate, PmxJoint.LowerTransLimit, PmxJoint.UpperTransLimit, PmxJoint.LowerRotateLimit, PmxJoint.UpperRotateLimit, PmxJoint.TransSpring, PmxJoint.RotateSpring
				}
			);
#endif
		}

		return true;
	}

	glm::vec3 CPmxImporter::CovertToZYX(const glm::vec3& Val)
	{
		return glm::vec3(Val.z, Val.y, Val.x);
	}
}
#endif // USE_MMD