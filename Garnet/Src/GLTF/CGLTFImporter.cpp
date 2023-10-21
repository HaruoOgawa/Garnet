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
#include "../Debug/Message/Console.h"
#include "../Math/CMath.h"

#include "../Animation/CAnimationClip.h"

namespace gltf
{
	bool CGLTFImporter::ImportFromMemory(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
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

		if (!Import(pGraphicsAPI, model, Object, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		return true;
	}

	bool CGLTFImporter::ImportFromString(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, const std::string& BaseDir, std::shared_ptr<object::C3DObject>& Object,
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
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

		if (!Import(pGraphicsAPI, model, Object, createInfo, TextureSet, DepthVertex, DepthFragment)) return false;

		return true;
	}

	bool CGLTFImporter::Import(api::IGraphicsAPI* pGraphicsAPI, tinygltf::Model model, std::shared_ptr<object::C3DObject>& Object,
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
		const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		// テクスチャ
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		if (!CreateTexture(pGraphicsAPI, model, TextureList)) return false;

		for (const auto& Texture : TextureList)
		{
			TextureSet->Add2DTexture(Texture);
		}

		// マテリアル
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		if (!CreateMaterial(pGraphicsAPI, model, MaterialList, createInfo, TextureSet)) return false;

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMesh(model, MeshList)) return false;

		// マテリアルを持っていないのならダミーを渡す
		if (MaterialList.size() <= 0)
		{
			if (!CreateDummyMaterial(pGraphicsAPI, MaterialList, createInfo, MeshList)) return false;
		}

		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;
		if (!CreateNode(model, NodeList, MeshList, MaterialList, RootNodeIndexList)) return false;

		// アニメーション
		std::vector<std::shared_ptr<animation::CAnimationClip>> AnimationClipList;
		if (!CreateAnimation(model, AnimationClipList, NodeList)) return false;

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

		for (const auto& Clip : AnimationClipList)
		{
			Object->AddAnimationClip(Clip);
		}

		Object->SetRootNodeIndexList(RootNodeIndexList);

		// オブジェクトを生成
		if (!Object->Create(pGraphicsAPI, DepthVertex, DepthFragment, TextureSet)) return false;

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
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
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
				UniformBuffer->AddData("baseColorFactor", &glm::vec4(baseColorFactor[0], baseColorFactor[1], baseColorFactor[2], baseColorFactor[3])[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("emissiveFactor", &glm::vec4(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2], 0.0f)[0], sizeof(float) * 4, 0);
				UniformBuffer->AddData("time", &glm::vec1(0.0f)[0], sizeof(float), 0);
				UniformBuffer->AddData("metallicFactor", &metallicFactor, sizeof(float), 0);
				UniformBuffer->AddData("roughnessFactor", &roughnessFactor, sizeof(float), 0);
				UniformBuffer->AddData("normalMapScale", &normalMapScale, sizeof(float), 0);
				UniformBuffer->AddData("occlusionStrength", &occlusionStrength, sizeof(float), 0);

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

				UniformBuffer->AddData("mipCount", &glm::vec1(MipCount)[0], sizeof(float), 0);

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

				UniformBuffer->AddData("ShadowMapX", &glm::vec1(static_cast<float>(ShadowMapX))[0], sizeof(float), 0);
				UniformBuffer->AddData("ShadowMapY", &glm::vec1(static_cast<float>(ShadowMapY))[0], sizeof(float), 0);

				// テクスチャを紐づける
				{
					//
					if (baseColorTextureIndex >= 0 && baseColorTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ "baseColorTexture", 1, 2, baseColorTextureIndex, graphics::ETextureUsage::TEXTURE_USAGE_2D});
						UniformBuffer->AddData("useBaseColorTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "baseColorTexture", 1, 2, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
						UniformBuffer->AddData("useBaseColorTexture", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					//
					if (metallicRoughnessTextureIndex >= 0 && metallicRoughnessTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ "metallicRoughnessTexture", 3, 4, metallicRoughnessTextureIndex, graphics::ETextureUsage::TEXTURE_USAGE_2D});
						UniformBuffer->AddData("useMetallicRoughnessTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "metallicRoughnessTexture", 3, 4, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
						UniformBuffer->AddData("useMetallicRoughnessTexture", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					//
					if (emissiveTextureIndex >= 0 && emissiveTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ "emissiveTexture", 5, 6, emissiveTextureIndex, graphics::ETextureUsage::TEXTURE_USAGE_2D});
						UniformBuffer->AddData("useEmissiveTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "emissiveTexture", 5, 6, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
						UniformBuffer->AddData("useEmissiveTexture", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					//
					if (normalTextureIndex >= 0 && normalTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ "normalTexture", 7, 8, normalTextureIndex, graphics::ETextureUsage::TEXTURE_USAGE_2D});
						UniformBuffer->AddData("useNormalTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "normalTexture", 7, 8, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
						UniformBuffer->AddData("useNormalTexture", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					//
					if (occlusionTextureIndex >= 0 && occlusionTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ "occlusionTexture", 9, 10, occlusionTextureIndex, graphics::ETextureUsage::TEXTURE_USAGE_2D});
						UniformBuffer->AddData("useOcclusionTexture", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "occlusionTexture", 9, 10, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
						UniformBuffer->AddData("useOcclusionTexture", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					// CubeMap
					if (CubeTexList.size() > 0)
					{
						material->AddTextureBindingLayout({ "cubemapTexture", 11, 12, 0, graphics::ETextureUsage::TEXTURE_USAGE_CUBE});
						UniformBuffer->AddData("useCubeMap", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "cubemapTexture", 11, 12, -1, graphics::ETextureUsage::TEXTURE_USAGE_CUBE });
						UniformBuffer->AddData("useCubeMap", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					// ShadowMap
					if(FrameTextureList.size() > 0)
					{
						// glTF FrameTextureList
						// [0] : ShadowMap
						// [1] : ???
						// [2] : ???

						// ひとまず末尾から取得
						material->AddTextureBindingLayout({ "shadowmapTexture", 13, 14, 0, graphics::ETextureUsage::TEXTURE_USAGE_FRAME});
						UniformBuffer->AddData("useShadowMap", &glm::uvec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "shadowmapTexture", 13, 14, -1, graphics::ETextureUsage::TEXTURE_USAGE_FRAME });
						UniformBuffer->AddData("useShadowMap", &glm::uvec1(0)[0], sizeof(int), 0);
					}

					// IBL
					if(Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
					{
						material->AddTextureBindingLayout({ "IBL_Diffuse_Texture", 15, 16, 0, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse });
						material->AddTextureBindingLayout({ "IBL_Specular_Texture", 17, 18, 0, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular });
						material->AddTextureBindingLayout({ "IBL_GGXLUT_Texture", 19, 20, 0, graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT });

						UniformBuffer->AddData("useIBL", &glm::ivec1(1)[0], sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ "IBL_Diffuse_Texture", 15, 16, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse });
						material->AddTextureBindingLayout({ "IBL_Specular_Texture", 17, 18, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular });
						material->AddTextureBindingLayout({ "IBL_GGXLUT_Texture", 19, 20, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT });

						UniformBuffer->AddData("useIBL", &glm::ivec1(0)[0], sizeof(int), 0);
					}
				}

				// マテリアルにUBOを割り当てる
				material->AddShaderBuffer(UniformBuffer);
			}

			// 登録
			MaterialList.push_back(material);
		}

		return true;
	}

	bool CGLTFImporter::CreateMesh(const tinygltf::Model& model, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
	{
		for (const auto& glTFMesh : model.meshes)
		{
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			for (const auto& glTFPrimitive : glTFMesh.primitives)
			{
				int MaterialIndex = glTFPrimitive.material;

				//
				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

				// 頂点バッファ本体
				std::vector<std::vector<float>> VertexDataList;
				std::vector<int> DimentionList;
				std::vector<unsigned short> Indices;
				std::vector<unsigned int> UINTIndices;

				// 頂点データの初期化用(例えばWeightとかNormalを持っていないならそれを0埋めするみたいな処理)
				std::vector<std::string> NeedAttribNameList = {
					"POSITION",
					"NORMAL",
					"TEXCOORD_0",
					"TANGENT",
				};
				std::map<std::string, std::vector<float>> ReservedVertexDataList;

				// タンジェントの計算が必要
				bool NeedRecalculateTangent = false;

				// 頂点バッファを読む
				{
					for (const auto& attribute : glTFPrimitive.attributes)
					{
						// アクセサーを取得
						std::string Name = attribute.first;
						int AccessorIndex = attribute.second;

						if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

						const auto& Accessor = model.accessors[AccessorIndex];
						int BufferViewIndex = Accessor.bufferView;
						size_t Count = Accessor.count;
						
						// Accessor_byteOffset: 複数のアクセサーがバッファビューを共有する場合に使用するそのバッファビュー内でのオフセットのこと
						size_t Accessor_byteOffset = Accessor.byteOffset;
						
						// 使用する型のバイト数. 5123のunsigned short か 5126のfloat
						int componentType = Accessor.componentType;
						int Stride = (componentType == 5126) ? 4 : 2;

						// SCALAR, VEC2, VEC3などがある 
						int Dimension = tinygltf::GetNumComponentsInType(Accessor.type);

						// byteLength: アクセサーのデータの長さ. (使用する型のバイト数, Stride) x (ディメンション) x (データ数)
						size_t byteLength = Stride * Dimension * Count;

						// バッファビューを取得
						if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

						const auto& BufferView = model.bufferViews[BufferViewIndex];

						int BufferIndex = BufferView.buffer;
						// アクセサー間でBufferViewを共有しつつもそのBufferViewをAccessorOffsetで分けて使用することもあるのでそれを考慮する
						size_t byteOffset = BufferView.byteOffset + Accessor_byteOffset; // アクセサーのオフセットを考慮する
						int target = BufferView.target;

						// データを取得
						std::vector<float> AttributeData;
						AttributeData.resize(byteLength / Stride);

						std::memcpy(&AttributeData[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);

						// データを登録
						ReservedVertexDataList.insert({ Name, AttributeData });
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
						else if (AttribName == "TANGENT" )
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
						}
					}
				}

				// インデックスバッファを読む
				{
					int AccessorIndex = glTFPrimitive.indices;

					if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

					const auto& Accessor = model.accessors[AccessorIndex];
					int BufferViewIndex = Accessor.bufferView;
					size_t Count = Accessor.count;
					int componentType = Accessor.componentType;
					int Stride = (componentType == 5125) ? 4 : 2;
					size_t Accessor_byteOffset = Accessor.byteOffset;
					size_t byteLength = Stride * Count;

					// バッファビューを取得
					if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

					const auto& BufferView = model.bufferViews[BufferViewIndex];

					int BufferIndex = BufferView.buffer;
					// アクセサー間でBufferViewを共有しつつもそのBufferViewをAccessorOffsetで分けて使用することもあるのでそれを考慮する
					size_t byteOffset = BufferView.byteOffset + Accessor_byteOffset;
					
					int target = BufferView.target;

					// データを取得
					if (componentType == 5123)
					{
						Indices.resize(byteLength / Stride);
						std::memcpy(&Indices[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);
					}
					else if(componentType == 5125)
					{
						UINTIndices.resize(byteLength / Stride);
						std::memcpy(&UINTIndices[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);
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
					}
				}

				// メッシュ情報を渡す
				createInfo->SetVertices(VertexDataList);
				createInfo->SetAttributeDimensions(DimentionList);

				if (Indices.size() > 0)
				{
					unsigned int size = math::CMath::CalcClosestPowerOfFour(static_cast<unsigned int>(Indices.size()));

					Indices.resize(size, 0);

					// Indicesを登録
					createInfo->SetIndices(Indices);
				}
				else if (UINTIndices.size() > 0)
				{
					unsigned int size = math::CMath::CalcClosestPowerOfFour(static_cast<unsigned int>(UINTIndices.size()));

					UINTIndices.resize(size, 0);

					// Indicesを登録
					createInfo->SetUINTIndices(UINTIndices);
				}

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
				Mesh->AddPrimitive(Primitive);
			}

			// メッシュを登録する
			MeshList.push_back(Mesh);
		}

		return true;
	}
	
	bool CGLTFImporter::CreateDummyMaterial(api::IGraphicsAPI* pGraphicsAPI, std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList,
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, std::vector<std::shared_ptr<graphics::CMesh>>& MeshList)
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
			material->AddTextureBindingLayout({ "baseColorTexture", 1, 2, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useBaseColorTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "metallicRoughnessTexture", 3, 4, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useMetallicRoughnessTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "emissiveTexture", 5, 6, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useEmissiveTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "normalTexture", 7, 8, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useNormalTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "occlusionTexture", 9, 10, -1, graphics::ETextureUsage::TEXTURE_USAGE_2D }); // TextureIndex -1 は EmptyTextureである
			UniformBuffer->AddData("useOcclusionTexture", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "cubemapTexture", 11, 12, -1, graphics::ETextureUsage::TEXTURE_USAGE_CUBE });
			UniformBuffer->AddData("useCubeMap", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "shadowmapTexture", 13, 14, -1, graphics::ETextureUsage::TEXTURE_USAGE_FRAME });
			UniformBuffer->AddData("useShadowMap", &glm::uvec1(0)[0], sizeof(int), 0);
			material->AddTextureBindingLayout({ "IBL_Diffuse_Texture", 15, 16, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse });
			material->AddTextureBindingLayout({ "IBL_Specular_Texture", 17, 18, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular });
			material->AddTextureBindingLayout({ "IBL_GGXLUT_Texture", 19, 20, -1, graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT });
			UniformBuffer->AddData("useIBL", &glm::ivec1(0)[0], sizeof(int), 0);

			// マテリアルにUBOを割り当てる
			material->AddShaderBuffer(UniformBuffer);
		}

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

	bool CGLTFImporter::CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		for (const auto& glTFNode : model.nodes)
		{
			// メッシュを持っていないノードもあることを考慮する必要がある
			int MeshIndex = glTFNode.mesh;

			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, MeshList, MaterialList);
			
			Node->SetName(glTFNode.name);

			const auto& scale = glTFNode.scale;
			if (scale.size() >= 3)
			{
				Node->SetScale(glm::vec3(scale[0], scale[1], scale[2]));
			}

			const auto& rotation = glTFNode.rotation;
			if (rotation.size() >= 3)
			{
				Node->SetRot(glm::vec3(rotation[0], rotation[1], rotation[2]));
			}

			const auto& position = glTFNode.translation;
			if (position.size() >= 3)
			{
				Node->SetPos(glm::vec3(position[0], position[1], position[2]));
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

				std::shared_ptr<animation::CAnimationChannel> AnimationChannel = std::make_shared<animation::CAnimationChannel>(sampler, AnimationTarget, Node);

				AnimationClip->AddAnimationChannel(AnimationChannel);
			}

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

		// timeAccessor
		{
			const auto& Accessor = model.accessors[input];

			std::vector<unsigned char> BufferData;
			if (!CalculateBufferFromAccessor(model, Accessor, BufferData)) return false;

			inputList.resize(BufferData.size() / 4);
			std::memcpy(&inputList[0], &BufferData[0], BufferData.size());
		}

		// translationAccessor
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

		// 使用する型のバイト数. 5123のunsigned short か 5126のfloat
		int componentType = Accessor.componentType;
		int Stride = (componentType == 5126) ? 4 : 2;

		// SCALAR, VEC2, VEC3などがある 
		// Accessor.typeで返ってくるのはタイプのenum indexのようなものでDimentionを取得するには以下の様にGetNumComponentsInTypeを使用する必要がある
		int Dimension = tinygltf::GetNumComponentsInType(Accessor.type);

		// byteLength: アクセサーのデータの長さ. (使用する型のバイト数, Stride) x (ディメンション) x (データ数)
		size_t byteLength = Stride * Dimension * Count;

		// バッファビューを取得
		if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) return false;

		const auto& BufferView = model.bufferViews[BufferViewIndex];

		int BufferIndex = BufferView.buffer;

		// アクセサー間でBufferViewを共有しつつもそのBufferViewをAccessorOffsetで分けて使用することもあるのでそれを考慮する
		size_t byteOffset = BufferView.byteOffset + Accessor_byteOffset; // アクセサーのオフセットを考慮する
		int target = BufferView.target;

		//
		BufferData.resize(byteLength);
		std::memcpy(&BufferData[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);

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
}
#endif // USE_GLTF