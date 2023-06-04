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

namespace gltf
{
	bool CGLTFImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data, std::shared_ptr<object::C3DObject>& Object,
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
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

		// テクスチャ
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList;
		if (!CreateTexture(pGraphicsAPI, model, TextureList)) return false;

		// マテリアル
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		if (!CreateMaterial(pGraphicsAPI, model, MaterialList, TextureList, createInfo)) return false;

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMesh(model, MeshList)) return false;
		
		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		if (!CreateNode(model, NodeList, MeshList, MaterialList)) return false;

		// オブジェクトにリソースを登録
		for (const auto& Texture : TextureList)
		{
			Object->AddTexture(Texture);
		}

		for (const auto& Material : MaterialList)
		{
			Object->AddMaterial(Material);
		}

		for (const auto& Node : NodeList)
		{
			Object->AddNode(Node);
		}

		// オブジェクトを生成
		if (!Object->Create(pGraphicsAPI)) return false;

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
		const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
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
			
			// Tex
			int baseColorTextureIndex = pbrParam.baseColorTexture.index;
			int metallicRoughnessTextureIndex = pbrParam.metallicRoughnessTexture.index;
			int emissiveTextureIndex = glTfMaterial.emissiveTexture.index;
			int normalTextureIndex = glTfMaterial.normalTexture.index;
			int occlusionTextureIndex = glTfMaterial.occlusionTexture.index;
			
			// マテリアルにシェーダーを設定
			std::shared_ptr<graphics::CMaterial> material = pGraphicsAPI->CreateMaterial();
			material->SetCreateInfo(createInfo);

			// UBO
			{
				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 0 });

				// UBOの初期値を設定する
				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("model", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("view", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::mat4 mat = glm::mat4(1.0f);
					UniformBuffer->AddData("proj", &mat[0][0], sizeof(mat), 0);
				}

				{
					glm::vec4 data = glm::vec4(0.0f);
					UniformBuffer->AddData("lightDir", &data[0], sizeof(float) * 4, 0);
				}
				
				{
					glm::vec4 data = glm::vec4(0.0f);
					UniformBuffer->AddData("lightColor", &data[0], sizeof(float) * 4, 0);
				}
				
				{
					glm::vec4 data = glm::vec4(0.0f);
					UniformBuffer->AddData("cameraPos", &data[0], sizeof(float) * 4, 0);
				}
				
				{
					glm::vec4 data = glm::vec4(baseColorFactor[0], baseColorFactor[1], baseColorFactor[2], baseColorFactor[3]);
					UniformBuffer->AddData("baseColorFactor", &data[0], sizeof(float) * 4, 0);
				}
				
				{
					glm::vec4 data = glm::vec4(emissiveFactor[0], emissiveFactor[1], emissiveFactor[2], 0.0f);
					UniformBuffer->AddData("emissiveFactor", &data[0], sizeof(float) * 4, 0);
				}

				{
					float val = 0.0f;
					UniformBuffer->AddData("time", &val, sizeof(float), 0);
					UniformBuffer->AddData("metallicFactor", &metallicFactor, sizeof(float), 0);
					UniformBuffer->AddData("roughnessFactor", &roughnessFactor, sizeof(float), 0);
					UniformBuffer->AddData("padding2", &val, sizeof(float), 0);
				}

				// テクスチャを紐づける
				{
					//
					if (baseColorTextureIndex >= 0 && baseColorTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 1, 2, baseColorTextureIndex });
					}

					//
					if (metallicRoughnessTextureIndex >= 0 && metallicRoughnessTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 3, 4, metallicRoughnessTextureIndex });
					}

					//
					if (emissiveTextureIndex >= 0 && emissiveTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 5, 6, emissiveTextureIndex });
					}

					//
					if (normalTextureIndex >= 0 && normalTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 7, 8, normalTextureIndex });
					}

					//
					if (occlusionTextureIndex >= 0 && occlusionTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 9, 10, occlusionTextureIndex });
					}
				}

				// オフセットの再計算
				UniformBuffer->RecalculateBindingLayoutOffset();

				// マテリアルにUBOを割り当てる
				material->AddUniformBuffer(UniformBuffer);
			}

			// テクスチャの割り当て
			
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

				// 頂点データの初期化用(例えばWeightとかNormalを持っていないならそれを0埋めするみたいな処理)
				std::map<std::string, std::vector<float>> ReservedVertexDataList;
				ReservedVertexDataList.insert({ "POSITION", std::vector<float>()});
				ReservedVertexDataList.insert({ "NORMAL", std::vector<float>()});
				ReservedVertexDataList.insert({ "TEXCOORD_0", std::vector<float>()});

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

						// バッファビューを取得
						if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

						const auto& BufferView = model.bufferViews[BufferViewIndex];

						int BufferIndex = BufferView.buffer;
						size_t byteOffset = BufferView.byteOffset;
						size_t byteLength = BufferView.byteLength;
						int target = BufferView.target;

						// データを取得
						std::vector<float> AttributeData;
						AttributeData.resize(byteLength / 4);

						std::memcpy(&AttributeData[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);

						// データを登録
						ReservedVertexDataList[Name] = AttributeData;
					}

					// 頂点バッファを再構築
					int VertexDataSize = static_cast<int>(ReservedVertexDataList["POSITION"].size()) / 3;
					for (auto& Data : ReservedVertexDataList)
					{
						std::string AttribName = Data.first;

						// ディメンションを取得
						int Dimention = 1;

						if (AttribName == "POSITION" || AttribName == "NORMAL")
						{
							Dimention = 3;
						}
						else if (AttribName == "TEXCOORD_0")
						{
							Dimention = 2;
						}

						DimentionList.push_back(Dimention);

						//
						if (Data.second.empty())
						{
							Data.second = std::vector<float>(VertexDataSize * Dimention, 0.0f);
						}
					}

					VertexDataList.push_back(ReservedVertexDataList["POSITION"]);
					VertexDataList.push_back(ReservedVertexDataList["NORMAL"]);
					VertexDataList.push_back(ReservedVertexDataList["TEXCOORD_0"]);
				}

				// インデックスバッファを読む
				{
					int AccessorIndex = glTFPrimitive.indices;

					if (AccessorIndex < 0 || AccessorIndex >= model.accessors.size()) continue;

					const auto& Accessor = model.accessors[AccessorIndex];
					int BufferViewIndex = Accessor.bufferView;
					size_t Count = Accessor.count;

					// バッファビューを取得
					if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

					const auto& BufferView = model.bufferViews[BufferViewIndex];

					int BufferIndex = BufferView.buffer;
					size_t byteOffset = BufferView.byteOffset;
					size_t byteLength = BufferView.byteLength;
					int target = BufferView.target;

					// データを取得
					Indices.resize(Count);
					std::memcpy(&Indices[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);
				}

				createInfo->SetVertices(VertexDataList);
				createInfo->SetIndices(Indices);
				createInfo->SetAttributeDimensions(DimentionList);

				// プリミティブを作成する
				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, MaterialIndex);
				Mesh->AddPrimitive(Primitive);
			}

			// メッシュを登録する
			MeshList.push_back(Mesh);
		}

		return true;
	}
	
	bool CGLTFImporter::CreateNode(const tinygltf::Model& model, std::vector<std::shared_ptr<object::CNode>>& NodeList, const std::vector<std::shared_ptr<graphics::CMesh>>& MeshList,
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList)
	{
		for (const auto& glTFNode : model.nodes)
		{
			//
			int MeshIndex = glTFNode.mesh;
			if (MeshIndex < 0 || MeshIndex >= MeshList.size()) continue;

			//
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshList[MeshIndex], MaterialList);
			
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

			NodeList.push_back(Node);
		}

		return true;
	}
}
#endif // USE_GLTF