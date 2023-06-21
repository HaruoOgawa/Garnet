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
		std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeTexList)
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

		// ひとまず仮で末尾にCubemapを追加
		for (const auto& Texture : CubeTexList)
		{
			TextureList.push_back(Texture);
		}

		// マテリアル
		std::vector<std::shared_ptr<graphics::CMaterial>> MaterialList;
		if (!CreateMaterial(pGraphicsAPI, model, MaterialList, TextureList, createInfo)) return false;

		// メッシュ
		std::vector<std::shared_ptr<graphics::CMesh>> MeshList;
		if (!CreateMesh(model, MeshList)) return false;
		
		// ノード
		std::vector<std::shared_ptr<object::CNode>> NodeList;
		std::vector<std::vector<int>> RootNodeIndexList;
		if (!CreateNode(model, NodeList, MeshList, MaterialList, RootNodeIndexList)) return false;

		// オブジェクトにリソースを登録
		for (const auto& Texture : TextureList)
		{
			Object->AddTexture(Texture);
		}

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

		Object->SetRootNodeIndexList(RootNodeIndexList);

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
			float normalMapScale = static_cast<float>(glTfMaterial.normalTexture.scale);
			float occlusionStrength = static_cast<float>(glTfMaterial.occlusionTexture.strength);

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

				// mat4
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

				// Vec4
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

				// Scaler
				{
					float val = 0.0f;
					UniformBuffer->AddData("time", &val, sizeof(float), 0);
					UniformBuffer->AddData("metallicFactor", &metallicFactor, sizeof(float), 0);
					UniformBuffer->AddData("roughnessFactor", &roughnessFactor, sizeof(float), 0);
					UniformBuffer->AddData("normalMapScale", &normalMapScale, sizeof(float), 0);

					UniformBuffer->AddData("occlusionStrength", &occlusionStrength, sizeof(float), 0);

					float mipCount = TextureList[TextureList.size() - 1]->GetMipCount();
					UniformBuffer->AddData("mipCount", &mipCount, sizeof(float), 0);
					
					UniformBuffer->AddData("s_pad1", &val, sizeof(float), 0);
					UniformBuffer->AddData("s_pad2", &val, sizeof(float), 0);
				}

				// テクスチャを紐づける
				{
					//
					if (baseColorTextureIndex >= 0 && baseColorTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 1, 2, baseColorTextureIndex });

						int Flag = 1;
						UniformBuffer->AddData("useBaseColorTexture", &Flag, sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ 1, 2, -1 }); // TextureIndex -1 は EmptyTextureである

						int Flag = 0;
						UniformBuffer->AddData("useBaseColorTexture", &Flag, sizeof(int), 0);
					}

					//
					if (metallicRoughnessTextureIndex >= 0 && metallicRoughnessTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 3, 4, metallicRoughnessTextureIndex });

						int Flag = 1;
						UniformBuffer->AddData("useMetallicRoughnessTexture", &Flag, sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ 3, 4, -1 }); // TextureIndex -1 は EmptyTextureである

						int Flag = 0;
						UniformBuffer->AddData("useMetallicRoughnessTexture", &Flag, sizeof(int), 0);
					}

					//
					if (emissiveTextureIndex >= 0 && emissiveTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 5, 6, emissiveTextureIndex });

						int Flag = 1;
						UniformBuffer->AddData("useEmissiveTexture", &Flag, sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ 5, 6, -1 }); // TextureIndex -1 は EmptyTextureである

						int Flag = 0;
						UniformBuffer->AddData("useEmissiveTexture", &Flag, sizeof(int), 0);
					}

					//
					if (normalTextureIndex >= 0 && normalTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 7, 8, normalTextureIndex });

						int Flag = 1;
						UniformBuffer->AddData("useNormalTexture", &Flag, sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ 7, 8, -1 }); // TextureIndex -1 は EmptyTextureである

						int Flag = 0;
						UniformBuffer->AddData("useNormalTexture", &Flag, sizeof(int), 0);
					}

					//
					if (occlusionTextureIndex >= 0 && occlusionTextureIndex < TextureList.size())
					{
						material->AddTextureBindingLayout({ 9, 10, occlusionTextureIndex });

						int Flag = 1;
						UniformBuffer->AddData("useOcclusionTexture", &Flag, sizeof(int), 0);
					}
					else
					{
						material->AddTextureBindingLayout({ 9, 10, -1 }); // TextureIndex -1 は EmptyTextureである

						int Flag = 0;
						UniformBuffer->AddData("useOcclusionTexture", &Flag, sizeof(int), 0);
					}

					// Cubemap, ひとまずTextureListの末尾に入れている
					{
						material->AddTextureBindingLayout({ 11, 12, (static_cast<int>(TextureList.size()) - 1) });
					}

					{
						int Flag = 0;
						UniformBuffer->AddData("t_pad_0", &Flag, sizeof(int), 0);
						UniformBuffer->AddData("t_pad_1", &Flag, sizeof(int), 0);
						UniformBuffer->AddData("t_pad_2", &Flag, sizeof(int), 0);
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
				std::vector<std::string> NeedAttribNameList = {
					"POSITION",
					"NORMAL",
					"TEXCOORD_0",
					"TANGENT",
					"BIOTANGENT",
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
						int Dimension = Accessor.type;

						// byteLength: アクセサーのデータの長さ. (使用する型のバイト数, Stride) x (ディメンション) x (データ数)
						size_t byteLength = Stride * Dimension * Count;

						// バッファビューを取得
						if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

						const auto& BufferView = model.bufferViews[BufferViewIndex];

						int BufferIndex = BufferView.buffer;
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
						else if (AttribName == "TANGENT" || AttribName == "BIOTANGENT")
						{
							Dimention = 4;
						}

						DimentionList.push_back(Dimention);

						// アトリビュートがまだ登録されていなければここで0埋めの値を渡す
						if (ReservedVertexDataList.find(AttribName) == ReservedVertexDataList.end())
						{
							ReservedVertexDataList.insert({ AttribName, std::vector<float>(VertexDataSize * Dimention, 0.0f) });

							// タンジェントの計算が必要
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
					int Stride = (componentType == 5126) ? 4 : 2;
					size_t Accessor_byteOffset = Accessor.byteOffset;
					size_t byteLength = Stride * Count;

					// バッファビューを取得
					if (BufferViewIndex < 0 || BufferViewIndex >= model.bufferViews.size()) continue;

					const auto& BufferView = model.bufferViews[BufferViewIndex];

					int BufferIndex = BufferView.buffer;
					size_t byteOffset = BufferView.byteOffset + Accessor_byteOffset;
					
					int target = BufferView.target;

					// データを取得
					Indices.resize(byteLength / Stride);
					std::memcpy(&Indices[0], &model.buffers[BufferIndex].data[byteOffset], byteLength);
				}

				// タンジェントの再計算
				if (NeedRecalculateTangent)
				{
					if (!RecalculateTangent(ReservedVertexDataList["TANGENT"], ReservedVertexDataList["BIOTANGENT"], ReservedVertexDataList["POSITION"], ReservedVertexDataList["TEXCOORD_0"], Indices)) return false;
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
		const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, std::vector<std::vector<int>>& RootNodeIndexList)
	{
		for (const auto& glTFNode : model.nodes)
		{
			// メッシュを持っていないノードもあることを考慮する必要がある
			int MeshIndex = glTFNode.mesh;

			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(MeshIndex, MeshList, MaterialList);
			
			Node->SetName(glTFNode.name);
			
			Node->SetMeshIndex(MeshIndex);

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

	// Helper Function
	bool CGLTFImporter::RecalculateTangent(std::vector<float>& TangentData, std::vector<float>& BioTangentData, const std::vector<float>& PosotionData, const std::vector<float>& TexcoordData, const std::vector<unsigned short>& Indices)
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

			BioTangentData[Index0 * 4 + 0] = BioTangent.x; BioTangentData[Index0 * 4 + 1] = BioTangent.y; BioTangentData[Index0 * 4 + 2] = BioTangent.z; BioTangentData[Index0 * 4 + 3] = BioTangent.w;
			BioTangentData[Index1 * 4 + 0] = BioTangent.x; BioTangentData[Index1 * 4 + 1] = BioTangent.y; BioTangentData[Index1 * 4 + 2] = BioTangent.z; BioTangentData[Index1 * 4 + 3] = BioTangent.w;
			BioTangentData[Index2 * 4 + 0] = BioTangent.x; BioTangentData[Index2 * 4 + 1] = BioTangent.y; BioTangentData[Index2 * 4 + 2] = BioTangent.z; BioTangentData[Index2 * 4 + 3] = BioTangent.w;
		}

		return true;
	}
}
#endif // USE_GLTF