#ifdef USE_GLTF

#include "CGLTFImporter.h"
#include "../Object/C3DObject.h"

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#endif // !TINYGLTF_IMPLEMENTATION

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif // !STB_IMAGE_WRITE_IMPLEMENTATION

#ifndef __EMSCRIPTEN__
#ifndef __STDC_LIB_EXT1__
#define __STDC_LIB_EXT1__
#endif // !__STDC_LIB_EXT1__
#endif // !__EMSCRIPTEN__

#include <tiny_gltf.h>
#include "../Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"
#include "../Animation/CSkeleton.h"
#include "../Animation/CBone.h"
#include "../Animation/CBoneNameProvider.h"

#include "../Graphics/CMaterialFrame.h"
#include "../../Graphics/CVertexBuffer.h"
#include "../../Graphics/CIndexBuffer.h"

#include "../../LoadWorker/CLoadWorker.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../LoadWorker/CTextureLoader.h"

namespace gltf
{
	bool CGLTFImporter::ImportFromMemory(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		// glTFのロード
		bool result = loader.LoadBinaryFromMemory(&model, &err, &warn, reinterpret_cast<const unsigned char*>(&Data[0]), static_cast<unsigned int>(Data.size()));

		if (!err.empty())
		{
			Console::Log("[glTF Error] %s\n", err.c_str());
		}
		
		if (!warn.empty())
		{
			Console::Log("[glTF Warning] %s\n", warn.c_str());
		}

		if (!result) return false;

		if (!Import(pGraphicsAPI, model, Object, MaterialFrame)) return false;

		return true;
	}

	bool CGLTFImporter::ImportFromString(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& BaseDir, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, resource::C3DObjectLoader* p3DObjectLoader)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		// glTFのロード
		bool result = loader.LoadASCIIFromString(&model, &err, &warn, reinterpret_cast<const char*>(&Data[0]), static_cast<unsigned int>(Data.size()), BaseDir);

		if (!err.empty())
		{
			Console::Log("[glTF Error] %s\n", err.c_str());
		}

		if (!warn.empty())
		{
			Console::Log("[glTF Warning] %s\n", warn.c_str());
		}

		if (!result) return false;

		if (!Import(pGraphicsAPI, model, Object, MaterialFrame))
		{
			Console::Log("[Error GLTFImporter] Failed to Import\n");

			return false;
		}

		return true;
	}

	bool CGLTFImporter::Import(api::IGraphicsAPI* pGraphicsAPI, tinygltf::Model model, object::C3DObject* Object,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		// テクスチャ
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		if (!CreateTexture(pGraphicsAPI, model, TextureList))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateTexture\n");

			return false;
		}

		for (const auto& Texture : TextureList)
		{
			Object->GetTextureSet()->Add2DTexture(Texture);
		}

		// マテリアル
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		if (!CreateMaterial(pGraphicsAPI, model, MaterialList, MaterialFrame, Object->GetTextureSet()))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateMaterial\n");

			return false;
		}

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMesh(pGraphicsAPI, model, MeshList))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateMesh\n");

			return false;
		}

		// マテリアルを持っていないのならダミーを渡す
		if (MaterialList.size() <= 0)
		{
			if (!CreateDummyMaterial(pGraphicsAPI, model, MaterialList, MaterialFrame, MeshList))
			{
				Console::Log("[Error GLTFImporter] Failed to CreateDummyMaterial\n");

				return false;
			}
		}

		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;
		if (!CreateNode(model, NodeList, RootNodeIndexList))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateNode\n");

			return false;
		}

		// スキン
		std::shared_ptr<animation::CSkeleton> Skeleton = std::make_shared<animation::CSkeleton>();
		if (!CreateAnimationSkeleton(model, Skeleton, NodeList))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateAnimationSkeleton\n");

			return false;
		}

		// NodeとSkeletonは先に追加しておく
		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		Object->SetAnimationSkeleton(Skeleton);

		Object->SetRootNodeIndexList(RootNodeIndexList);

		// DefaultLocalTransformを保存する
		Object->ApplyDefaultLocalTransform();

		// 親ノードを設定
		Object->ApplyParentNode();

		// ワールド行列の計算
		Object->CalcWorldMatrix();

		// 親のBoneを追加
		if(Skeleton)
		{
			ApplyParentBoneList(Skeleton, NodeList);
		}

		// アニメーション
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;
		if (!CreateAnimation(model, AnimationClipList, NodeList))
		{
			Console::Log("[Error GLTFImporter] Failed to CreateAnimation\n");

			return false;
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

		for (const auto& Clip : AnimationClipList)
		{
			Object->AddAnimationClip(Clip);
		}

		// モーフノードを追加
		for (const auto& Node : NodeList)
		{
			int MeshIndex = Node->GetMeshIndex();
			if (MeshIndex < 0 || MeshIndex >= MeshList.size()) continue;

			const auto& Mesh = MeshList[MeshIndex];

			if (Mesh->GetMorphDataList().size() > 0)
			{
				Object->AddMorphNode(Node);
			}
		}

		return true;
	}

	bool CGLTFImporter::CreateTexture(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CTexture>>& TextureList)
	{
		for (const auto& glTFTexture : model.textures)
		{
			// テクスチャサンプラーの形式を選択(まだ未実装)
			const auto& sampler = glTFTexture.sampler;

			// テクスチャを取得
			const auto& source = glTFTexture.source;
			if (source < 0 || source >= model.images.size()) continue;
			
			const auto& image = model.images[source];

			if (image.uri.empty())
			{
				// uriが定義されていないのならbufferViewから取得する
				int bufferViewIndex = image.bufferView;
				if (bufferViewIndex < 0 || bufferViewIndex >= model.bufferViews.size()) continue;

				const auto& bufferView = model.bufferViews[bufferViewIndex];
				int bufferIndex = bufferView.buffer;
				size_t byteOffset = bufferView.byteOffset;
				size_t byteLength = bufferView.byteLength;

				if (bufferIndex < 0 || bufferIndex >= model.buffers.size()) continue;
				const auto& buffer = model.buffers[bufferIndex];
				std::vector<unsigned char> textureData;
				textureData.resize(byteLength);

				std::memcpy(&textureData[0], &buffer.data[byteOffset], byteLength);

				std::shared_ptr<graphics::CTexture> Texture = pGraphicsAPI->CreateTexture();
				if (!Texture->Create(textureData)) return false;

				// 登録する
				TextureList.push_back(Texture);
			}
			else
			{
				// uriが定義されているのでディレクトリからテクスチャを取得する
			}
		}

		return true;
	}

	bool CGLTFImporter::CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		//
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList(0);
		if (TextureSet) TextureList = TextureSet->Get2DTextureList();
		
		std::vector<std::shared_ptr<graphics::CTexture>> CubeTexList(0);
		if (TextureSet) CubeTexList = TextureSet->GetCubeMapList();
		
		std::vector<std::shared_ptr<graphics::CTexture>> FrameTextureList(0);
		if (TextureSet) FrameTextureList = TextureSet->GetFrameTextureList();

		std::shared_ptr<graphics::CTexture> Diffuse_Tex = nullptr;
		if (TextureSet) Diffuse_Tex = TextureSet->GetDiffuse_Tex();

		std::shared_ptr<graphics::CTexture> Specular_Tex = nullptr;
		if (TextureSet) Specular_Tex = TextureSet->GetSpecular_Tex();

		std::shared_ptr<graphics::CTexture> GGXLUT_Tex = nullptr;
		if (TextureSet) GGXLUT_Tex = TextureSet->GetGGXLUT_Tex();

		//
		for (const auto& glTfMaterial : model.materials)
		{
			//
			const auto& pbrParam = glTfMaterial.pbrMetallicRoughness;

			// Vec4
			const auto& baseColorFactor = glTfMaterial.pbrMetallicRoughness.baseColorFactor;
			const auto& emissiveFactor = glTfMaterial.emissiveFactor;
			
			// Scaler
			float metallicFactor = static_cast<float>(pbrParam.metallicFactor);
			float roughnessFactor = static_cast<float>(pbrParam.roughnessFactor);
			float normalMapScale = static_cast<float>(glTfMaterial.normalTexture.scale);
			float occlusionStrength = static_cast<float>(glTfMaterial.occlusionTexture.strength);

			// Tex
			int baseColorTextureIndex = pbrParam.baseColorTexture.index;
			int metallicRoughnessTextureIndex = pbrParam.metallicRoughnessTexture.index;
			int emissiveTextureIndex = glTfMaterial.emissiveTexture.index;
			int normalTextureIndex = glTfMaterial.normalTexture.index;
			int occlusionTextureIndex = glTfMaterial.occlusionTexture.index;
			
			// マテリアルにシェーダーを設定
			std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI, 1, graphics::ECullMode::CULL_BACK);

			// UBO
			{
				// UBOの初期値を設定する
				material->ReplacePreloadUniformValue("baseColorFactor", &glm::vec4(baseColorFactor[0], baseColorFactor[1], baseColorFactor[2], baseColorFactor[3])[0], sizeof(float) * 4, 0);
				material->ReplacePreloadUniformValue("emissiveFactor", &glm::vec4(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2], 0.0f)[0], sizeof(float) * 4, 0);
				material->ReplacePreloadUniformValue("metallicFactor", &metallicFactor, sizeof(float), 0);
				material->ReplacePreloadUniformValue("roughnessFactor", &roughnessFactor, sizeof(float), 0);
				material->ReplacePreloadUniformValue("normalMapScale", &normalMapScale, sizeof(float), 0);
				material->ReplacePreloadUniformValue("occlusionStrength", &occlusionStrength, sizeof(float), 0);

				// MipCountには反射キューブマップかIBLのSpecularMapの値が入っている(これらは必ずどちらか一方しか使用されないため)
				float MipCount = 1.0f;
				if (CubeTexList.size() > 0)
				{
					MipCount = CubeTexList[0]->GetMipCount();
				}
				else if(Specular_Tex)
				{
					MipCount = Specular_Tex->GetMipCount();
				}

				material->ReplacePreloadUniformValue("mipCount", &glm::vec1(MipCount)[0], sizeof(float), 0);

				int ShadowMapX = 1, ShadowMapY = 1;
				if (FrameTextureList.size() > 0)
				{
					// glTF FrameTextureList
					// [0] : ShadowMap
					// [1] : ???
					// [2] : ???
					ShadowMapX = FrameTextureList[0]->GetWidth();
					ShadowMapY = FrameTextureList[0]->GetHeight();
				}

				material->ReplacePreloadUniformValue("ShadowMapX", &glm::vec1(static_cast<float>(ShadowMapX))[0], sizeof(float), 0);
				material->ReplacePreloadUniformValue("ShadowMapY", &glm::vec1(static_cast<float>(ShadowMapY))[0], sizeof(float), 0);

				// テクスチャを紐づける
				{
					if (baseColorTextureIndex >= 0 && baseColorTextureIndex < TextureList.size())
					{
						material->ReplaceTextureIndex("baseColorTexture", baseColorTextureIndex);
						material->ReplacePreloadUniformValue("useBaseColorTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					if (metallicRoughnessTextureIndex >= 0 && metallicRoughnessTextureIndex < TextureList.size())
					{
						material->ReplaceTextureIndex("metallicRoughnessTexture", metallicRoughnessTextureIndex);
						material->ReplacePreloadUniformValue("useMetallicRoughnessTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					if (emissiveTextureIndex >= 0 && emissiveTextureIndex < TextureList.size())
					{
						material->ReplaceTextureIndex("emissiveTexture", emissiveTextureIndex);
						material->ReplacePreloadUniformValue("useEmissiveTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					
					if (normalTextureIndex >= 0 && normalTextureIndex < TextureList.size())
					{
						material->ReplaceTextureIndex("normalTexture", normalTextureIndex);
						material->ReplacePreloadUniformValue("useNormalTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					if (occlusionTextureIndex >= 0 && occlusionTextureIndex < TextureList.size())
					{
						material->ReplaceTextureIndex("occlusionTexture", occlusionTextureIndex);
						material->ReplacePreloadUniformValue("useOcclusionTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					// CubeMap
					if (CubeTexList.size() > 0)
					{
						material->ReplaceTextureIndex("cubemapTexture", 0);
						material->ReplacePreloadUniformValue("useCubeMap", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					// ShadowMap
					if (FrameTextureList.size() > 0)
					{
						// glTF FrameTextureList
						// [0] : ShadowMap
						// [1] : ???
						// [2] : ???

						// ひとまず末尾から取得
						material->ReplaceTextureIndex("shadowmapTexture", 0);
						material->ReplacePreloadUniformValue("useShadowMap", &glm::uvec1(1)[0], sizeof(int), 0);
					}

					// IBL
					if (Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
					{
						material->ReplaceTextureIndex("IBL_Diffuse_Texture", 0);
						material->ReplaceTextureIndex("IBL_Specular_Texture", 0);
						material->ReplaceTextureIndex("IBL_GGXLUT_Texture", 0);

						material->ReplacePreloadUniformValue("useIBL", &glm::ivec1(1)[0], sizeof(int), 0);
					}
				}

				material->ReplacePreloadUniformValue("useSkinMeshAnimation", &glm::ivec1(0)[0], sizeof(int), 0);
				
			}

			// SkinMatrix StorageBuffer
			{
				int SkinMatCount = 0;
				for (const auto& glTFSkeleton : model.skins) { SkinMatCount += static_cast<int>(glTFSkeleton.joints.size()); }

				// DynamicOffsetが256バイトからしか使えない都合上SkinMatCountの最小値は4とする(4 * 16 * 4 = 256)
				if(SkinMatCount < 4) SkinMatCount = 4;

				// SSBOのサイズは2のn乗である必要がある
				SkinMatCount = math::CMath::CalcNextPowerOfTwo(SkinMatCount);

				std::vector<glm::mat4> SkinMatrixList;
				SkinMatrixList.resize(SkinMatCount, glm::mat4(0.0f));

				material->ReplacePreloadUniformValue("r_SkinMatrixBuffer", &SkinMatrixList[0], static_cast<int>(SkinMatrixList.size()) * sizeof(glm::mat4), 1);
			}

			// 登録
			MaterialList.push_back(material);
		}

		return true;
	}

	bool CGLTFImporter::CreateMesh(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		for (const auto& glTFMesh : model.meshes)
		{
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			for (const auto& glTFPrimitive : glTFMesh.primitives)
			{
				int MaterialIndex = glTFPrimitive.material;

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

				// タンジェントの計算が必要
				bool NeedRecalculateTangent = false;

				// モーフデータ
				std::vector<std::map<int, glm::vec3>> MorphDataList;

				// 頂点バッファを読む
				{
					for (const auto& attribute : glTFPrimitive.attributes)
					{
						// アクセサーを取得
						std::string Name = attribute.first;
						int AccessorIndex = attribute.second;

						if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

						const auto& Accessor = model.accessors[AccessorIndex];

						// 使用する型のバイト数. 5123のunsigned short、5126のfloat など
						// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types
						int Stride = CalcStrideFromAccessor(model, Accessor);

						// データを取得
						std::vector<unsigned char> BufferData;
						if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

						std::vector<float> AttributeData;
						AttributeData.resize(BufferData.size() / Stride);
						std::memcpy(&AttributeData[0], &BufferData[0], BufferData.size());

						// データを登録
						ReservedVertexDataList.insert({ Name, AttributeData });

						// コンポーネントタイプ(データ型)を取得
						graphics::EDataType attribComponentType = GetComponentTypeFromAccessor(Accessor);
						ReservedDataTypeList.insert({ Name, attribComponentType });

						// ByteStrideを取得
						// byteStrideとは「１つ分」のデータと、次の「1つ分」のデータとの間の、読み取り場所の移動バイト長
						// http://muko.damember.org/gl4/html-ja/glVertexAttribPointer.xhtml
						int attibByteStride = GetByteStride(model, Accessor);
						ReservedByteStrideList.insert({ Name, attibByteStride });
					}

					// モーフターゲット
					for (int MorphIndex = 0; MorphIndex < static_cast<int>(glTFPrimitive.targets.size()); MorphIndex++)
					{
						const auto& glTFMorph = glTFPrimitive.targets[MorphIndex];

						// POSITIONのモーフのみに対応する
						{
							auto it = glTFMorph.find("POSITION");
							if (it != glTFMorph.end())
							{
								int AccessorIndex = (*it).second;

								if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

								const auto& Accessor = model.accessors[AccessorIndex];

								// 使用する型のバイト数. 5123のunsigned short、5126のfloat など
								// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types
								int Stride = CalcStrideFromAccessor(model, Accessor);

								// データを取得
								std::vector<unsigned char> BufferData;
								if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

								std::vector<float> AttributeData;
								AttributeData.resize(BufferData.size() / Stride);
								std::memcpy(&AttributeData[0], &BufferData[0], BufferData.size());

								//
								{
									std::map<int, glm::vec3> MorphData;
									int MorphVertexIndex = 0;

									for (int MorphAttibIndex = 0; MorphAttibIndex < AttributeData.size(); MorphAttibIndex += 3)
									{
										glm::vec3 Offset = glm::vec3(AttributeData[MorphAttibIndex + 0], AttributeData[MorphAttibIndex + 1], AttributeData[MorphAttibIndex + 2]);

										MorphData.emplace(MorphVertexIndex, Offset);

										MorphVertexIndex++;
									}

									MorphDataList.push_back(MorphData);
								}
							}
						}
					}

					// アトリビュートがまだ登録されていなければここで0埋めの値を渡す
					int VertexDataSize = static_cast<int>(ReservedVertexDataList["POSITION"].size()) / 3;
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
							ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexDataSize * Dimention, 0.0f) });

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

				// インデックスバッファを読む
				{
					int AccessorIndex = glTFPrimitive.indices;
					if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

					const auto& Accessor = model.accessors[AccessorIndex];
					int componentType = Accessor.componentType;
					int Stride = (componentType == 5125) ? 4 : 2;

					// データを取得
					std::vector<unsigned char> BufferData;
					if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;
					
					if (componentType == 5123)
					{
						Indices.resize(BufferData.size() / Stride);
						std::memcpy(&Indices[0], &BufferData[0], BufferData.size());
					}
					else if(componentType == 5125)
					{
						UINTIndices.resize(BufferData.size() / Stride);
						std::memcpy(&UINTIndices[0], &BufferData[0], BufferData.size());
					}
				}

				// タンジェントの再計算
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

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(VertexBuffer, IndexBuffer, MaterialIndex);
				Mesh->SetMorphDataList(Primitive, static_cast<int>(Mesh->GetPrimitiveList().size()), MorphDataList);
				Mesh->AddPrimitive(Primitive);
			}

			// メッシュを登録する
			MeshList.push_back(Mesh);
		}

		return true;
	}
	
	bool CGLTFImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		// マテリアル参照数とマテリアルインデックスの設定
		int MatRefCount = 0;
		for (auto& Mesh : MeshList)
		{
			for (auto& Primirive : Mesh->GetPrimitiveList())
			{
				Primirive->SetMaterialIndex(0);
				MatRefCount++;
			}
		}

		// マテリアルにシェーダーを設定
		std::shared_ptr<graphics::CMaterial> material = MaterialFrame->CreateMaterial(pGraphicsAPI, MatRefCount, graphics::ECullMode::CULL_NONE);
		
		MaterialList.push_back(material);

		return true;
	}

	bool CGLTFImporter::CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		for (const auto& glTFNode : model.nodes)
		{
			// メッシュを持っていないノードもあることを考慮する必要がある
			int MeshIndex = glTFNode.mesh;
			int SkeletonIndex = glTFNode.skin;

			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, static_cast<int>(NodeList.size()));
			
			std::string NodeName = glTFNode.name;
			if (NodeName.empty())
			{
				NodeName = "node_" + std::to_string(static_cast<int>(NodeList.size()));
			}

			Node->SetName(NodeName);
			Node->SetSkeletonIndex(SkeletonIndex);

			const auto& glTFMatrix = glTFNode.matrix;

			if (glTFMatrix.size() == 16)
			{
				// From Double Vector To Float Vector
				std::vector<float> matrix(glTFMatrix.size());
				std::transform(glTFMatrix.begin(), glTFMatrix.end(), matrix.begin(), [](double val) {return static_cast<float>(val); });

				// matrixが存在するのでそれからTransformを復元する
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				std::memcpy(&modelMatrix[0][0], &matrix[0], sizeof(float) * matrix.size());

				// 受け取ったデータが行優先なのでglmの列優先に変換
				modelMatrix = glm::transpose(modelMatrix);

				//
				glm::vec3 Pos = glm::vec3(0.0f);
				glm::quat Rotation = glm::quat();
				glm::vec3 Scale = glm::vec3(0.0f);

				math::CTransform::CastModelMatrixToTransform(modelMatrix, Pos, Rotation, Scale);

				math::CTransform::CastLeftHandToRightHand(Pos);
				math::CTransform::CastLeftHandToRightHand(Rotation);

				Node->SetPos(Pos);
				Node->SetRot(Rotation);
				Node->SetScale(Scale);
			}
			else
			{
				const auto& scale = glTFNode.scale;
				if (scale.size() == 3)
				{
					Node->SetScale(glm::vec3(static_cast<float>(scale[0]), static_cast<float>(scale[1]), static_cast<float>(scale[2])));
				}

				const auto& rotation = glTFNode.rotation;
				if (rotation.size() == 4)
				{
					// glmのクォータニオンは wxyzで指定する必要がある？
					glm::quat quat = glm::quat(static_cast<float>(rotation[3]), static_cast<float>(rotation[0]), static_cast<float>(rotation[1]), static_cast<float>(rotation[2]));
					Node->SetRot(quat);
				}

				const auto& position = glTFNode.translation;
				if (position.size() == 3)
				{
					glm::vec3 pos = glm::vec3(static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2]));
					Node->SetPos(pos);
				}
			}

			Node->SetChildrenNodeIndexList(glTFNode.children);

			NodeList.push_back(Node);
		}

		// scenesのnodesはルートノードを示すのでそこから走破をスタートする必要がある(たぶん以前glTFアニメーションがうまくいかなかったのはこれが原因. それと親要素から子要素ではなく子要素から親要素に走破していたのも原因かも)
		for (const auto& scene : model.scenes)
		{
			RootNodeIndexList.push_back(scene.nodes);
		}

		return true;
	}

	bool CGLTFImporter::CreateAnimationSkeleton(const tinygltf::Model& model, std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& glTFSkeleton : model.skins)
		{
			// inverseBindMatrices
			std::vector<glm::mat4> inverseBindMatrices;
			{
				int Accessor_Index = glTFSkeleton.inverseBindMatrices;
				if (Accessor_Index < 0 || Accessor_Index >= model.accessors.size()) continue;

				const auto& Accessor = model.accessors[Accessor_Index];

				std::vector<unsigned char> BufferData;
				if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

				inverseBindMatrices.resize(BufferData.size() / sizeof(glm::mat4));

				std::memcpy(&inverseBindMatrices[0], &BufferData[0], BufferData.size());
			}

			// Bones
			for (const auto& glTFBone : glTFSkeleton.joints)
			{
				if (glTFBone < 0 || glTFBone >= NodeList.size()) continue;

				const auto& BoneNode = NodeList[glTFBone];

				// 同じ名前のボーンでもリストに追加する必要がある
				// JointIndexの順番がそれも込みで設定されているため
				// BrainStemがそのことを示している
				/*// 既に存在するボーンかチェックする
				bool Exist = false;
				for (const auto& Bone : Skeleton->GetBoneList())
				{
					if (Bone->GetBoneNode()->GetName() == BoneNode->GetName())
					{
						// 存在する
						Exist = true;

						break;
					}
				}

				if (Exist) continue;*/

				std::shared_ptr<animation::CBone> Bone = std::make_shared<animation::CBone>(BoneNode);

				Skeleton->AddBone(Bone);
			}

			// Add InverseBindMatrix To Bone
			for (int j = 0; j < Skeleton->GetBoneList().size(); j++)
			{
				// Boneの順番とinverseBindMatrixの順番は同じ
				const auto& Bone = Skeleton->GetBoneList()[j];
				Bone->GetBoneNode()->SetInverseBindMatrix(inverseBindMatrices[j]);
			}
		}

		// Humanoid Boneを持っていればBoneに割り当てる
		const auto& VRM = model.extensions.find("VRM");
		if (VRM != model.extensions.end())
		{
			if (VRM->second.IsObject() && VRM->second.Has("humanoid"))
			{
				const auto& humanoid = VRM->second.Get("humanoid");

				if (humanoid.IsObject() && humanoid.Has("humanBones"))
				{
					const auto& humanBones = humanoid.Get("humanBones");

					if (humanBones.IsArray())
					{
						for (int BoneIndex = 0; BoneIndex < humanBones.ArrayLen(); BoneIndex++)
						{
							const auto& bone = humanBones.Get(BoneIndex);

							if (bone.IsObject())
							{
								// 拡張情報を取得
								std::string name = bone.Get("bone").Get<std::string>();
								int nodeIndex = bone.Get("node").Get<int>();

								// BoneNameを取得
								std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
								animation::EHumanoidBones BoneName = Provider->GetBoneName(name);

								// BoneにBoneNameを割り当てる
								if (nodeIndex >= 0 && nodeIndex < NodeList.size())
								{
									const auto& TargetNode = NodeList[nodeIndex];

									if(Skeleton)
									{
										for (const auto& Bone : Skeleton->GetBoneList())
										{
											if (Bone->GetBoneNode() == TargetNode)
											{
												Bone->SetBoneName(BoneName);

												break;
											}
										}
									}
								}
							}
						}
					}
				}
				
			}
		}

		// 拡張機能の結果を元にBoneTableを作成
		if(Skeleton)
		{
			Skeleton->MakeBoneTable();
		}

		return true;
	}


	void CGLTFImporter::ApplyParentBoneList(const std::shared_ptr<animation::CSkeleton>& Skeleton, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& Bone : Skeleton->GetBoneList())
		{
			const auto& ParentNode = Bone->GetBoneNode()->GetParentNode();
			if (!ParentNode) continue;

			std::shared_ptr<animation::CBoneNameProvider> Provider = std::make_shared<animation::CBoneNameProvider>();
			animation::EHumanoidBones ParentBoneName = Provider->GetBoneName(ParentNode->GetName());

			const auto& ParentBone = Skeleton->GetBone(ParentBoneName);
			if (!ParentBone) continue;

			Bone->SetParentBoneName(ParentBone->GetBoneName());
		}
	}

	bool CGLTFImporter::CreateAnimation(const tinygltf::Model& model, std::vector<std::shared_ptr<animation::CAnimationClip>>& AnimationClipList, const std::vector<std::shared_ptr<object::CNode>>& NodeList)
	{
		for (const auto& glTFAnimation : model.animations)
		{
			std::shared_ptr<animation::CAnimationClip> AnimationClip = std::make_shared<animation::CAnimationClip>();

			// samplers
			for (const auto& glTFSampler : glTFAnimation.samplers)
			{
				std::shared_ptr<animation::CAnimationSampler> AnimationSampler = nullptr;
				if (!CreateAnimationSampler(model, glTFSampler, AnimationSampler)) return false;

				AnimationClip->AddAnimationSampler(AnimationSampler);
			}

			// channels
			for (const auto& glTFChannel : glTFAnimation.channels)
			{
				// アニメーションのローカル軸を使用するか
				// FBXでは必須でglTF/VRMでは不要
				// 詳しくはCAnimationChannel::UpdateRotationを参照
				const bool UseAnimLocalAxis = false;

				int sampler = glTFChannel.sampler;
				int target_node = glTFChannel.target_node;
				const std::string& target_path = glTFChannel.target_path;

				const auto& Node = NodeList[target_node];

				animation::EAnimationTarget AnimationTarget = animation::EAnimationTarget::NONE;

				if (target_path == "translation")
				{
					AnimationTarget = animation::EAnimationTarget::TRANSLATION;
				}
				else if (target_path == "rotation")
				{
					AnimationTarget = animation::EAnimationTarget::ROTATION;
				}
				else if (target_path == "scale")
				{
					AnimationTarget = animation::EAnimationTarget::SCALE;
				}
				else if (target_path == "weights")
				{
					AnimationTarget = animation::EAnimationTarget::WEIGHTS;
				}

				animation::EHumanoidBones BoneName = animation::EHumanoidBones::None;

				std::shared_ptr<animation::CAnimationChannel> AnimationChannel = std::make_shared<animation::CAnimationChannel>(UseAnimLocalAxis, false, sampler, AnimationTarget, Node, BoneName);

				AnimationClip->AddAnimationChannel(AnimationChannel);
			}

			AnimationClip->SetIsLoop(true);

			AnimationClipList.push_back(AnimationClip);
		}

		return true;
	}

	bool CGLTFImporter::CreateAnimationSampler(const tinygltf::Model& model, const tinygltf::AnimationSampler& glTFSampler, std::shared_ptr<animation::CAnimationSampler>& AnimationSampler)
	{
		std::vector<float> inputList;
		std::vector<float> outputList;

		int input = glTFSampler.input;
		const std::string& interpolation = glTFSampler.interpolation;
		int output = glTFSampler.output;

		// SCALAR, VEC2, VEC3などがある 
		int Accessor_type = -1;

		// inputAccessor
		{
			const auto& Accessor = model.accessors[input];

			std::vector<unsigned char> BufferData;
			if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

			inputList.resize(BufferData.size() / 4);
			std::memcpy(&inputList[0], &BufferData[0], BufferData.size());
		}

		// outputAccessor
		{
			const auto& Accessor = model.accessors[output];

			Accessor_type = Accessor.type;

			std::vector<unsigned char> BufferData;
			if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

			outputList.resize(BufferData.size() / 4);
			std::memcpy(&outputList[0], &BufferData[0], BufferData.size());
		}

		//
		animation::EInterpolationType InterpolationType = animation::EInterpolationType::NONE;
		if (interpolation == "STEP")
		{
			InterpolationType = animation::EInterpolationType::STEP;
		}
		else if (interpolation == "LINEAR")
		{
			InterpolationType = animation::EInterpolationType::LINEAR;
		}
		else if (interpolation == "CUBICSPLINE")
		{
			InterpolationType = animation::EInterpolationType::CUBICSPLINE;
		}

		// Make Sampler Object
		AnimationSampler = std::make_shared<animation::CAnimationSampler>(InterpolationType);
		if (!AnimationSampler->CreateKeyFrame(ConvertToEKeyFrameType(Accessor_type), inputList, outputList)) return false;

		return true;
	}

	// Helper Function ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CGLTFImporter::CalculateBufferFromAccessor(const tinygltf::Model& model, const tinygltf::Accessor& Accessor, std::vector<unsigned char>& BufferData)
	{
		int BufferViewIndex = Accessor.bufferView;
		size_t Count = Accessor.count;

		// Accessor_byteOffset: 複数のアクセサーがバッファビューを共有する場合に使用するそのバッファビュー内でのオフセットのこと
		size_t Accessor_byteOffset = Accessor.byteOffset;

		// 使用する型のバイト数. 5123のunsigned short、5126のfloat など
		// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#accessor-data-types
		int Stride = CalcStrideFromAccessor(model, Accessor);

		// SCALAR, VEC2, VEC3などがある 
		// Accessor.typeで返ってくるのはタイプのenum indexのようなものでDimentionを取得するには以下の様にGetNumComponentsInTypeを使用する必要がある
		int Dimension = tinygltf::GetNumComponentsInType(Accessor.type);

		// byteLength: アクセサーのデータの長さ. (使用する型のバイト数, Stride) x (ディメンション) x (データ数)
		// BufferViewerは異なるデータ間で共有されることがあるのでそのbyteLengthは取得したいデータそのものの長さとは限らない
		// なのでアクセサーのデータの長さを優先する
		size_t Accessor_byteLength = Stride * Dimension * Count;

		// バッファビューを取得
		if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) return false;

		const auto& BufferView = model.bufferViews[BufferViewIndex];

		int BufferIndex = BufferView.buffer;

		// アクセサー間でBufferViewを共有しつつもそのBufferViewをAccessorOffsetで分けて使用することもあるのでそれを考慮する
		size_t byteOffset = BufferView.byteOffset + Accessor_byteOffset; // アクセサーのオフセットを考慮する

		// バッファデータを取得
		BufferData.resize(Accessor_byteLength);
		std::memcpy(&BufferData[0], &model.buffers[BufferIndex].data[byteOffset], Accessor_byteLength);

		return true;
	}

	bool CGLTFImporter::RecalculateTangent(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices)
	{
		for (int i = 0; i < Indices.size(); i += 3)
		{
			// 頂点情報を取得
			unsigned short Index0 = Indices[i + 0], Index1 = Indices[i + 1], Index2 = Indices[i + 2];

			glm::vec3 Pos0 = glm::vec3(PosotionData[Index0 * 3 + 0], PosotionData[Index0 * 3 + 1], PosotionData[Index0 * 3 + 2]);
			glm::vec3 Pos1 = glm::vec3(PosotionData[Index1 * 3 + 0], PosotionData[Index1 * 3 + 1], PosotionData[Index1 * 3 + 2]);
			glm::vec3 Pos2 = glm::vec3(PosotionData[Index2 * 3 + 0], PosotionData[Index2 * 3 + 1], PosotionData[Index2 * 3 + 2]);

			glm::vec2 Texcoord0  = glm::vec2(TexcoordData[Index0 * 2 + 0], TexcoordData[Index0 * 2 + 1]);
			glm::vec2 Texcoord1  = glm::vec2(TexcoordData[Index1 * 2 + 0], TexcoordData[Index1 * 2 + 1]);
			glm::vec2 Texcoord2  = glm::vec2(TexcoordData[Index2 * 2 + 0], TexcoordData[Index2 * 2 + 1]);

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

	bool CGLTFImporter::RecalculateTangentWithUINT(std::vector<float>& TangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned int>& Indices)
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

	animation::EKeyFrameType CGLTFImporter::ConvertToEKeyFrameType(int Type)
	{
		if (Type == TINYGLTF_TYPE_SCALAR)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_SCALAR;
		}
		else if (Type == TINYGLTF_TYPE_VEC2)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_VEC2;
		}
		else if (Type == TINYGLTF_TYPE_VEC3)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_VEC3;
		}
		else if (Type == TINYGLTF_TYPE_VEC4)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_VEC4;
		}
		else if (Type == TINYGLTF_TYPE_MAT2)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_MAT2;
		}
		else if (Type == TINYGLTF_TYPE_MAT3)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_MAT3;
		}
		else if (Type == TINYGLTF_TYPE_MAT4)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_MAT4;
		}
		else if (Type == TINYGLTF_TYPE_VECTOR)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_VECTOR;
		}
		else if (Type == TINYGLTF_TYPE_MATRIX)
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_MATRIX;
		}
		else
		{
			return animation::EKeyFrameType::KEYFRAME_TYPE_NONE;
		}
	}

	int CGLTFImporter::CalcStrideFromAccessor(const tinygltf::Model& model, const tinygltf::Accessor& Accessor)
	{
		size_t byteStride = GetByteStride(model, Accessor);
		
		if (byteStride != 0)
		{
			// byteStrideがあればそちらを優先する
			int Dimension = tinygltf::GetNumComponentsInType(Accessor.type);

			int Stride = static_cast<int>(byteStride) / Dimension;

			return Stride;
		}
		else
		{
			// componentTypeからStrideを取得
			int componentType = Accessor.componentType;

			if (componentType == 5120)
			{
				// signed byte
				return 1;
			}
			else if (componentType == 5121)
			{
				// unsigned byte
				return 1;
			}
			else if (componentType == 5122)
			{
				// signed short
				return 2;
			}
			else if (componentType == 5123)
			{
				// unsigned short
				return 2;
			}
			else if (componentType == 5125)
			{
				// unsigned int
				return 4;
			}
			else if (componentType == 5126)
			{
				// float
				return 4;
			}
			else
			{
				// Unknown
				return -1;
			}
		}
	}

	graphics::EDataType CGLTFImporter::GetComponentTypeFromAccessor(const tinygltf::Accessor& Accessor)
	{
		int componentType = Accessor.componentType;

		if (componentType == 5120)
		{
			// signed byte
			return graphics::EDataType::TYPE_SIGNED_BYTE;
		}
		else if (componentType == 5121)
		{
			// unsigned byte
			return graphics::EDataType::TYPE_UNSIGNED_BYTE;
		}
		else if (componentType == 5122)
		{
			// signed short
			return graphics::EDataType::TYPE_SIGNED_SHORT;
		}
		else if (componentType == 5123)
		{
			// unsigned short
			return graphics::EDataType::TYPE_UNSIGNED_SHORT;
		}
		else if (componentType == 5125)
		{
			// unsigned int
			return graphics::EDataType::TYPE_UNSIGNED_INT;
		}
		else if (componentType == 5126)
		{
			// float
			return graphics::EDataType::TYPE_FLOAT;
		}
		else
		{
			// Unknown
			return graphics::EDataType::TYPE_FLOAT;
		}
	}

	int CGLTFImporter::GetByteStride(const tinygltf::Model& model, const tinygltf::Accessor& Accessor)
	{
		int byteStride = 0;

		int BufferViewIndex = Accessor.bufferView;
		if (BufferViewIndex >= 0 && BufferViewIndex < model.bufferViews.size())
		{
			const auto& BufferView = model.bufferViews[BufferViewIndex];

			byteStride = static_cast<int>(BufferView.byteStride);
		}

		return byteStride;
	}
}
#endif // USE_GLTF