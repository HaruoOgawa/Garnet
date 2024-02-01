#ifdef USE_MMD
#include "CPmxImporter.h"
#include "../../Debug/Message/Console.h"
#include "../../Math/CMath.h"
#include "../../Math/CTransform.h"
#include "../../Object/C3DObject.h"
#include "../../Animation/CAnimationClip.h"
#include "../../Animation/CSkin.h"
#include "../../Animation/CJoint.h"
#include "../../Animation/CBoneNameProvider.h"

#include "../../Graphics/CMaterialFrame.h"

#include "../../LoadWorker/CLoadWorker.h"
#include "../../LoadWorker/CTextureLoader.h"

namespace mmd
{
	bool CPmxImporter::ImportPmx(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::string& ModelFileName, const std::vector<unsigned char>& Data, object::C3DObject* Object,
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

		std::shared_ptr<object::CNode> RootNode = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));
		RootNode->SetName("RootNode");
		RootNode->SetU16Name(L"RootNode");
		NodeList.push_back(RootNode);

		RootNodeIndexList.push_back(std::vector<int>({ 0 }));

		// Skin
		std::shared_ptr<animation::CSkin> Skin = std::make_shared<animation::CSkin>();
		if (!CreateAnimationSkin(model, Skin, NodeList, RootNode)) return false;

		// BoneTableを作成
		Skin->MakeBoneTable();

		// マテリアルリスト
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		if (!CreateMaterialList(pGraphicsAPI, model, MaterialList, MaterialFrame, Skin)) return false;

		// テクスチャリスト
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		std::vector<std::shared_ptr<resource::IResource>> RuntimeLoadResourceList;

		if (!CreateTextureList(pGraphicsAPI, pLoadWorker, ModelFileName, model, TextureList, RuntimeLoadResourceList)) return false;

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMeshList(model, MeshList, RootNode, NodeList, MaterialList, (Skin->GetJointList().size() > 0))) return false;

		// リソースを登録
		Object->SetRootNodeIndexList(RootNodeIndexList);

		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		Object->AddAnimationSkin(Skin);

		for (const auto& Material : MaterialList)
		{
			Object->AddMaterial(Material);
		}

		for (const auto& Texture : TextureList)
		{
			Object->GetTextureSet()->Add2DTexture(Texture);
		}

		for (const auto& Resource : RuntimeLoadResourceList)
		{
			Object->AddRuntimeLoadResource(Resource);
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
		if (!CalcInverseBindPose(Skin)) return false;

		return true;
	}

	bool CPmxImporter::CreateAnimationSkin(const CPmxModel& model, std::shared_ptr<animation::CSkin>& Skin, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::shared_ptr<object::CNode>& RootNode)
	{
		// PmxではBoneとJointは全くの別物でそれぞれ違う役割を持っているので厳格に名前分けする必要がある!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		const auto& PmxBoneList = model.GetPmxBoneList();

		for (int BoneIndex = 0; BoneIndex < PmxBoneList.size(); BoneIndex++)
		{
			const auto& PmxBone = PmxBoneList[BoneIndex];

			// BoneNodeの作成
			std::shared_ptr<object::CNode> BoneNode = std::make_shared<object::CNode>(-1, static_cast<int>(NodeList.size()));

			const auto& Name = PmxBone->GetBoneName().second;
			BoneNode->SetU16Name(Name);

			glm::vec3 Pos = PmxBone->GetPos();

			// PMXはRotateは持っていないのでひとまず0にする
			glm::quat Rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

			// PMXのPos・Rotateはワールド座標系での値なので親ノードのワールドマトリックスを乗算してローカル座標系に戻す必要がある
			int ParentBoneIndex = PmxBone->GetParentBoneIndex();
			if(ParentBoneIndex >= 0 && ParentBoneIndex < PmxBoneList.size())
			{
				const auto& ParentPmxBone = PmxBoneList[ParentBoneIndex];
				
				// 親ノードの分だけ移動させる 
				Pos -= ParentPmxBone->GetPos();
			}

			//
			BoneNode->SetPos(Pos);
			BoneNode->SetRot(Rot);

			BoneNode->SaveAsDefaultLocalTransform();

			NodeList.push_back(BoneNode);

			// Boneを作成
			std::shared_ptr<animation::CJoint> Bone = std::make_shared<animation::CJoint>(BoneNode);

			// BoneにBoneNameを割り当てる
			animation::CBoneNameProvider Provider;
			animation::EHumanoidBones BoneName = Provider.GetBoneNameU16(Name);
			Bone->SetBoneName(BoneName);

			Skin->AddJoint(Bone);
		}

		// BoneNodeに子要素を設定する
		{
			const auto& BoneList = Skin->GetJointList();

			for (int BoneIndex = 0; BoneIndex < PmxBoneList.size(); BoneIndex++)
			{
				const auto& PmxBone = PmxBoneList[BoneIndex];

				const auto& Bone = BoneList[BoneIndex];
				int SelfNodeIndex = Bone->GetJointNode()->GetSelfNodeIndex();

				int ParentBoneIndex = PmxBone->GetParentBoneIndex();

				/// 範囲外を示すときはRootNodeを親に持つ
				if (ParentBoneIndex < 0 || ParentBoneIndex >= BoneList.size())
				{
					RootNode->AddChildrenNodeIndex(SelfNodeIndex);
				}
				else
				{
					// 自身を親ノードの子要素リストに追加する
					BoneList[ParentBoneIndex]->GetJointNode()->AddChildrenNodeIndex(SelfNodeIndex);
				}
			}
		}

		return true;
	}

	bool CPmxImporter::CalcInverseBindPose(std::shared_ptr<animation::CSkin>& Skin)
	{
		for (const auto& Bone : Skin->GetJointList())
		{
			// MMDのBoneはローカル座標系ではなくワールド座標系なのでセンターとかの親ボーンを考慮するかは迷うところ
			glm::mat4 InverseBindMatrix = glm::inverse(Bone->GetJointNode()->GetWorldMatrix());
			Bone->GetJointNode()->SetInverseBindMatrix(InverseBindMatrix);
		}

		return true;
	}

	bool CPmxImporter::CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<animation::CSkin>& Skin)
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
					material->ReplacePreloadUniformValue("UseToonTexture", &glm::ivec1(1)[0], sizeof(int), 2);
					material->ReplaceTextureIndex("ToonTexture", SharedToonTexIndex);
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
				// SkinMatは存在するJointの数だけ用意する必要がある
				// DynamicOffsetが256バイトからしか使えない都合上SkinMatCountの最小値は4とする(4 * 16 * 4 = 256)
				unsigned int SkinMatCount = 0;
				if (Skin && Skin->GetJointList().size() > 0) SkinMatCount = static_cast<unsigned int>(Skin->GetJointList().size());

				// DynamicOffsetが256バイトからしか使えない都合上SkinMatCountの最小値は4とする(4 * 16 * 4 = 256)
				if (SkinMatCount < 4) SkinMatCount = 4;

				// SSBOのサイズは2のn乗である必要がある
				SkinMatCount = math::CMath::CalcNextPowerOfTwo(SkinMatCount);

				std::vector<glm::mat4> SkinMatrixList;
				SkinMatrixList.resize(SkinMatCount, glm::mat4(1.0f));

				material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
			}

			MaterialList.push_back(material);
		}

		return true;
	}

	bool CPmxImporter::CreateMeshList(const CPmxModel& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList, const std::shared_ptr<object::CNode>& RootNode, std::vector<std::shared_ptr<object::CNode>>& NodeList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, bool ExistSkin)
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
			// 頂点バッファ本体
			std::vector<std::vector<float>> VertexDataList;
			std::vector<int> DimentionList;
			std::vector<renderer::EDataType> DataTypeList;
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
					ReservedByteStrideList.emplace("POSITION", 0);
				}

				{
					ReservedVertexDataList.emplace("NORMAL", PmxMesh->GetNormalAttribute());
					ReservedDataTypeList.emplace("NORMAL", renderer::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("NORMAL", 0);
				}

				{
					ReservedVertexDataList.emplace("TEXCOORD_0", PmxMesh->GetUVAttribute());
					ReservedDataTypeList.emplace("TEXCOORD_0", renderer::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TEXCOORD_0", 0);
				}

				{
					std::vector<float> AttributeData;
					AttributeData.resize(4 * (PmxMesh->GetPositionAttribute().size() / 3), 0.0f);

					ReservedVertexDataList.emplace("TANGENT", AttributeData);
					ReservedDataTypeList.emplace("TANGENT", renderer::EDataType::TYPE_FLOAT);
					ReservedByteStrideList.emplace("TANGENT", 0);
				}

				{
					std::vector<float> AttributeData;

					if (MetaData.BoneIndexSize == 1)
					{
						const auto& ByteJointAttribute = PmxMesh->GetByteJointAttribute();

						if (!ByteJointAttribute.empty())
						{
							AttributeData.resize(ByteJointAttribute.size() / 4);
							std::memcpy(&AttributeData[0], &ByteJointAttribute[0], sizeof(unsigned char) * ByteJointAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_BYTE);
						ReservedByteStrideList.emplace("JOINTS_0", 1 * 4);
					}
					else if (MetaData.BoneIndexSize == 2)
					{
						const auto& UShortJointAttribute = PmxMesh->GetUShortJointAttribute();

						if (!UShortJointAttribute.empty())
						{
							AttributeData.resize(UShortJointAttribute.size() / 2);
							std::memcpy(&AttributeData[0], &UShortJointAttribute[0], sizeof(unsigned short) * UShortJointAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_SHORT);
						ReservedByteStrideList.emplace("JOINTS_0", 2 * 4);
					}
					else if (MetaData.BoneIndexSize == 4)
					{
						const auto& IntJointAttribute = PmxMesh->GetUIntJointAttribute();

						if (!IntJointAttribute.empty())
						{
							AttributeData.resize(IntJointAttribute.size());
							std::memcpy(&AttributeData[0], &IntJointAttribute[0], sizeof(unsigned int) * IntJointAttribute.size());
						}

						ReservedDataTypeList.emplace("JOINTS_0", renderer::EDataType::TYPE_UNSIGNED_INT);
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
					ReservedDataTypeList.emplace("WEIGHTS_0", renderer::EDataType::TYPE_FLOAT);
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

			const auto& PmxMaterialList = model.GetPmxMaterialList();

			int MatRefOffset = 0;

			for (int MaterialIndex = 0; MaterialIndex < PmxMaterialList.size(); MaterialIndex++)
			{
				const auto& PmxMaterial = PmxMaterialList[MaterialIndex];

				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

				// メッシュを作成する
				std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

				// メッシュ情報を渡す
				createInfo->SetVertices(VertexDataList);
				createInfo->SetAttributeDimensions(DimentionList);
				createInfo->SetAttribDataTypes(DataTypeList);
				createInfo->SetAttribByteStrides(ByteStrideList);

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
						createInfo->SetIndices(Indices);
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
						createInfo->SetUINTIndices(UINTIndices);
					}

					// 参照オフセットを更新する
					MatRefOffset += PmxMaterial->GetMatRefIndiceCount();
				}

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
				Mesh->AddPrimitive(Primitive);

				// メッシュを登録
				MeshList.push_back(Mesh);

				// PMXにはノードの概念がないのでこちらで明示的に作成する
				int MeshIndex = static_cast<int>(MeshList.size()) - 1;
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, static_cast<int>(NodeList.size()));

				Node->SetU16Name(PmxMaterial->GetMaterialName().second);

				// ひとまずPMXはSkinを1つしか持っていない
				int SkinIndex = (ExistSkin) ? 0 : -1;
				Node->SetSkinIndex(SkinIndex);

				NodeList.push_back(Node);

				// MeshNodeに子要素を登録する
				std::vector<int> ChildrenNodeIndexList = MeshNode->GetChildrenNodeIndexList();
				ChildrenNodeIndexList.push_back(static_cast<int>(NodeList.size()) - 1);

				MeshNode->SetChildrenNodeIndexList(ChildrenNodeIndexList);
			}
		}

		return true;
	}

	bool CPmxImporter::CreateTextureList(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::string& ModelFileName, const CPmxModel& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList,
		std::vector<std::shared_ptr<resource::IResource>>& RuntimeLoadResourceList)
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
			RuntimeLoadResourceList.push_back(TexLoader);
			//pLoadWorker->AddFirstLoadResource(TexLoader);
			pLoadWorker->AddRuntimeLoadResource(TexLoader);
		}

		return true;
	}
}
#endif // USE_MMD