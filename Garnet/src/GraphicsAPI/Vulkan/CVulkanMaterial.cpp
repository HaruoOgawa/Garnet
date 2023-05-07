#ifndef __DAWN__
#include "CVulkanMaterial.h"
#include "CVulkanAPI.h"
#include "../CMaterialCreateInfo.h"

namespace api
{
	CVulkanMaterial::CVulkanMaterial():
		CMaterial(),
		m_pGraphicsAPI(nullptr),

		m_VertShaderModule(nullptr),
		m_FragShaderModule(nullptr),

		m_DescriptorSetLayout(nullptr),
		m_DescriptorPool(nullptr),

		m_UseMainTexture(false),
		m_TextureImage(nullptr),
		m_TextureImageMemory(nullptr),
		m_TextureImageView(nullptr),
		m_TextureSampler(nullptr)
	{
	}

	CVulkanMaterial::~CVulkanMaterial()
	{
		Release();
	}

	bool CVulkanMaterial::Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);
		m_UseMainTexture = createInfo.IsUseMainTexture();

		if (!CreateShaderStages(createInfo)) return false; // Shaderの作成

		/*if (!CreateTextureImage(createInfo)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView(createInfo)) return false;// シェーダーで取り扱う用のImageViewを作成
		if (!CreateTextureSampler(createInfo)) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構*/

		if (!CreateDescriptorSetLayout(createInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
		if (!CreateUniformBuffers(createInfo)) return false; // ユニフォームバッファを作成
		if (!CreateDescriptorPool(createInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
		if (!CreateDescriptorSets(createInfo)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/

		// シェーダーモジュールを破棄する
		/*vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), m_FragShaderModule, nullptr);
		vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), m_VertShaderModule, nullptr);*/

		return true;
	}

	bool CVulkanMaterial::Update(float SecondsTime)
	{
		// ユニフォームデータの更新
		UpdateUniformBuffer(m_pGraphicsAPI->GetCurrentFrame(), SecondsTime);

		return true;
	}

	void CVulkanMaterial::UpdateUniformBuffer(uint32_t CurrentImage, float SecondsTime)
	{
		VkDeviceSize bufferSize = sizeof(float) * 16 * 4 + sizeof(float) * 4 * 4;

		//
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), SecondsTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			m_pGraphicsAPI->GetSwapChainExtent().width / (float)m_pGraphicsAPI->GetSwapChainExtent().height, 0.1f, 10.0f
		);
		proj[1][1] *= -1.0f; // Y座標の向きを反転。VulkanとOpenGLは逆なのかな？
		glm::mat4 mvp = proj * view * model;

		//
		std::vector<float> testUBO = {
			0.01f, 0.01f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f
		};

		//
		std::vector<float> Data;
		Data.resize(16 * 4 + testUBO.size());
		std::memcpy(&Data[16 * 0], &model[0][0], sizeof(float) * 16);
		std::memcpy(&Data[16 * 1], &view[0][0], sizeof(float) * 16);
		std::memcpy(&Data[16 * 2], &proj[0][0], sizeof(float) * 16);
		std::memcpy(&Data[16 * 3], &mvp[0][0], sizeof(float) * 16);

		std::memcpy(&Data[16 * 4], &testUBO[0], sizeof(float) * 4 * 4);

		// 空の値を既にマップしているのでVulkan関数を使わなくても値がコピーできる
		//std::memcpy(m_UniformBuffersMapped[CurrentImage], &Data[0], bufferSize);
	}

	void CVulkanMaterial::Release()
	{
		if (m_UseMainTexture)
		{
			// テクスチャサンプラーを破棄
			vkDestroySampler(m_pGraphicsAPI->GetLogicalDevice(), m_TextureSampler, nullptr);
			m_TextureSampler = nullptr;

			// テクスチャ用のイメージビューの破棄
			vkDestroyImageView(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageView, nullptr);
			m_TextureImageView = nullptr;

			// テクスチャイメージの破棄
			vkDestroyImage(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImage, nullptr);
			m_TextureImage = nullptr;
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageMemory, nullptr);
			m_TextureImageMemory = nullptr;
		}

		// ユニフォームの破棄
		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (auto& Buffer : m_UniformBuffersList[i])
			{
				vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
			}
			
			for (auto& Memory : m_UniformBuffersMemoryList[i])
			{
				vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), Memory, nullptr);
			}
		}

		// 記述子プールの破棄
		vkDestroyDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorPool, nullptr);
		m_DescriptorPool = nullptr;

		// ユニフォームレイアウトセットを破棄
		vkDestroyDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
		m_DescriptorSetLayout = nullptr;
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanMaterial::CreateShaderStages(const graphics::CMaterialCreateInfo& createInfo)
	{
		// シェーダーの準備
		// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
		// 使う時にGeometryとかTessellationも追加する
		const auto& VertexShaderData = createInfo.GetVertexShaderCode();
		const bool UseVertexShader = CreateShaderModule(m_VertShaderModule, std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

		const auto& FragmentShaderCode = createInfo.GetFragmentShaderCode();
		const bool UseFragmentShader = CreateShaderModule(m_FragShaderModule, std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));

		// シェーダーステージの作成(VertexShaderとかFragment, Geometryとかそういうステージ)
		if (UseVertexShader)
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = m_VertShaderModule;
			vertShaderStageInfo.pName = "main";

			m_ShaderStages.push_back(vertShaderStageInfo);
		}

		if (UseFragmentShader)
		{
			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = m_FragShaderModule;
			fragShaderStageInfo.pName = "main";

			m_ShaderStages.push_back(fragShaderStageInfo);
		}

		return true;
	}

	bool CVulkanMaterial::CreateTextureImage(const graphics::CMaterialCreateInfo& createInfo)
	{
		if (createInfo.IsUseMainTexture())
		{
			// テクスチャをロード
			int texWidth = 1024, texHeight = 1024, texChannels;
			//stbi_uc* pixels = stbi_load("src/Textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			VkDeviceSize imageSize = texWidth * texHeight * 4;

			/*if (!pixels)
			{
				return false;
			}*/

			// テクスチャイメージのステージングバッファを作成
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			m_pGraphicsAPI->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferMemory);

			void* data; // <-- dataにGPUへの入り口のポインタが渡される(あとで閉じられる) -> そこにテクスチャデータをコピーする(GPUのバッファに渡される)
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
			//std::memcpy(data, pixels, static_cast<size_t>(imageSize));
			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

			// ロードしたピクセルデータはもう不要なので解放する
			//stbi_image_free(pixels);

			// テクスチャイメージオブジェクトを生成
			m_pGraphicsAPI->CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);

			// イメージテクスチャのレイアウトを別形式へ移行する --> バッファにコピー可な形式に変換
			m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			// ステージングバッファのデータをテクスチャイメージへコピーする
			m_pGraphicsAPI->CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

			// イメージテクスチャのレイアウトを別形式へ移行する --> シェーダーで読み込み可な形式に変換
			m_pGraphicsAPI->TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			// ステージングバッファの破棄
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);
		}

		return true;
	}

	bool CVulkanMaterial::CreateTextureImageView(const graphics::CMaterialCreateInfo& createInfo)
	{
		if (createInfo.IsUseMainTexture())
		{
			m_TextureImageView = m_pGraphicsAPI->CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		return true;
	}

	bool CVulkanMaterial::CreateTextureSampler(const graphics::CMaterialCreateInfo& createInfo)
	{
		if (createInfo.IsUseMainTexture())
		{
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(m_pGraphicsAPI->GetPhysicalDevice(), &properties);
			samplerInfo.anisotropyEnable = VK_TRUE; // 異方性フィルタリング --> 遠くの方のテクスチャがぼけてしまうのを調整する機
			samplerInfo.maxAnisotropy = properties.limits.maxSamplerAllocationCount;

			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			if (vkCreateSampler(m_pGraphicsAPI->GetLogicalDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
			{
				return false;
			}
		}

		return true;
	}

	bool CVulkanMaterial::CreateDescriptorSetLayout(const graphics::CMaterialCreateInfo& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		// レイアウトのバインドに関する設定
		for (const auto& Buffer : createInfo.GetBufferList())
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				VkDescriptorSetLayoutBinding LayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
				LayoutBinding.binding = Layout.BindingIndex; // バインディングインデックス
				
				switch (Buffer->GetBufferType())
				{
					case graphics::EBufferType::UNIFROM:
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // バッファタイプ
						break;
				
					case graphics::EBufferType::TEXTURE:
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // バッファタイプ
						break;

					default:
						break;
				}
				
				LayoutBinding.descriptorCount = 1; // 
				LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
				LayoutBinding.pImmutableSamplers = nullptr; // 画像のサンプリングに使用するフィールド

				bindings.push_back(LayoutBinding);
			}
		}

		// Model, View Proj等のUBOのレイアウト
		/*VkDescriptorSetLayoutBinding uboLayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
		uboLayoutBinding.binding = 0; // バインディングインデックス？ 
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // バッファタイプ
		uboLayoutBinding.descriptorCount = 1; // 
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
		uboLayoutBinding.pImmutableSamplers = nullptr; // 画像のサンプリングに使用するフィールド

		bindings.push_back(uboLayoutBinding);

		//
		VkDescriptorSetLayoutBinding testUBOLayoutBinding{};
		testUBOLayoutBinding.binding = 1;
		testUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		testUBOLayoutBinding.descriptorCount = 1;
		testUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		testUBOLayoutBinding.pImmutableSamplers = nullptr;

		bindings.push_back(testUBOLayoutBinding);

		// TextureSampler用のレイアウトを設定
		if (createInfo.IsUseMainTexture())
		{
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = 1; //バインディングインデックス. 上のやつが0だから1を設定
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // バッファタイプ
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			bindings.push_back(samplerLayoutBinding);
		}*/

		// レイアウトの作成に関する設定
		VkDescriptorSetLayoutCreateInfo layoutInfo{}; // : bindingをまとめるためのオブジェクト
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);

		return (result == VK_SUCCESS);
	}

	bool CVulkanMaterial::CreateUniformBuffers(const graphics::CMaterialCreateInfo& createInfo)
	{
		VkDeviceSize bufferSize = sizeof(float) * 16 * 4 + sizeof(float) * 4 * 4;

		m_UniformBuffersList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_UniformBuffersMemoryList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_UniformBuffersMappedList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (const auto& Buffer : createInfo.GetBufferList())
			{
				VkBuffer UniformBuffer;
				VkDeviceMemory BufferMemory;
				void* BuffersMappedList;

				m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);

				// 後で書き込むのでひとまず空でマップする
				vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), BufferMemory, 0, bufferSize, 0, &BuffersMappedList);

				m_UniformBuffersList[i].push_back(UniformBuffer);
				m_UniformBuffersMemoryList[i].push_back(BufferMemory);
				m_UniformBuffersMappedList[i].push_back(BuffersMappedList);
			}
		}

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorPool(const graphics::CMaterialCreateInfo& createInfo)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (const auto& Buffer : createInfo.GetBufferList())
		{
			VkDescriptorPoolSize poolSize{};

			switch (Buffer->GetBufferType())
			{
			case graphics::EBufferType::UNIFROM:
				poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;

			case graphics::EBufferType::TEXTURE:
				poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;

			default:
				break;
			}

			poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

			poolSizes.push_back(poolSize);
		}

		/* {
			VkDescriptorPoolSize poolSize{};

			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

			poolSizes.push_back(poolSize);
		}

		if (createInfo.IsUseMainTexture())
		{
			VkDescriptorPoolSize poolSize{};

			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

			poolSizes.push_back(poolSize);
		}*/


		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

		if (vkCreateDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorSets(const graphics::CMaterialCreateInfo& createInfo)
	{
		std::vector<VkDescriptorSetLayout> layouts(m_pGraphicsAPI->GetMaxFramesInFlight(), m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());
		allocInfo.pSetLayouts = layouts.data();

		//
		m_DescriptorSets.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		if (vkAllocateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			return false;
		}

		//
		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			for (int n = 0; n < createInfo.GetBufferList().size(); n++)
			{
				const auto& Buffer = createInfo.GetBufferList()[n];

				for (const auto& Layout : Buffer->GetBindingLayoutList())
				{
					VkWriteDescriptorSet descriptorWrite{};

					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = m_DescriptorSets[i]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrite.dstBinding = Layout.BindingIndex; // layout(location = n)
					descriptorWrite.dstArrayElement = 0; // ???
					
					switch (Buffer->GetBufferType())
					{
					case graphics::EBufferType::UNIFROM:
						{
							VkDescriptorBufferInfo bufferInfo{};
							bufferInfo.buffer = m_UniformBuffersList[i][n]; // UBOの指定
							bufferInfo.offset = Layout.ByteOffset; // バッファオフセット
							bufferInfo.range = Layout.ByteSize; // サイズかな？

							descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // どのタイプのコマンドを発行してもらうのか
							descriptorWrite.descriptorCount = 1;
							descriptorWrite.pBufferInfo = &bufferInfo;
						}
						break;

					case graphics::EBufferType::TEXTURE:
						{
							VkDescriptorImageInfo imageInfo{};
							imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							imageInfo.imageView = m_TextureImageView;
							imageInfo.sampler = m_TextureSampler;

							descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							descriptorWrite.descriptorCount = 1;
							descriptorWrite.pImageInfo = &imageInfo;
						}
						break;

					default:
						break;
					}

					descriptorWrites.push_back(descriptorWrite);
				}
			}

			/*// UBO用
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i]; // UBOの指定
			bufferInfo.offset = 0; // でた、バッファオフセット!!!!!
			bufferInfo.range = sizeof(float) * 16 * 4; // サイズかな？

			//
			VkDescriptorBufferInfo testBufferInfo{};
			testBufferInfo.buffer = m_UniformBuffers[i];
			testBufferInfo.offset = sizeof(float) * 16 * 4;;
			testBufferInfo.range = sizeof(float) * 4 * 4;

			// テクスチャサンプラー用
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_TextureImageView;
			imageInfo.sampler = m_TextureSampler;

			//
			std::vector<VkWriteDescriptorSet> descriptorWrites{};

			// UniformBufferSet
			{
				VkWriteDescriptorSet descriptorWrite{};

				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
				descriptorWrite.dstBinding = 0; // layout(location = n)
				descriptorWrite.dstArrayElement = 0; // ???
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // どのタイプのコマンドを発行してもらうのか
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;

				descriptorWrites.push_back(descriptorWrite);
			}

			// testUBOSet
			{
				VkWriteDescriptorSet descriptorWrite{};

				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = 1;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &testBufferInfo;

				descriptorWrites.push_back(descriptorWrite);
			}

			// ImageBufferSet
			if (createInfo.IsUseMainTexture())
			{
				VkWriteDescriptorSet descriptorWrite{};

				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = 1;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pImageInfo = &imageInfo;

				descriptorWrites.push_back(descriptorWrite);
			}*/

			vkUpdateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		return true;
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader
	// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
	bool CVulkanMaterial::CreateShaderModule(VkShaderModule& shaderModule, const std::string& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}
#endif