#ifndef __DAWN__
#include "CVulkanMaterial.h"
#include "CVulkanAPI.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"

#include "../../Math/CTransform.h"

namespace api
{
	CVulkanMaterial::CVulkanMaterial():
		CMaterial(),
		m_pGraphicsAPI(nullptr),

		m_VertShaderModule(nullptr),
		m_FragShaderModule(nullptr),

		m_DescriptorSetLayout(nullptr),
		m_DescriptorPool(nullptr),

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

		if (!CreateShaderStages(createInfo)) return false; // Shaderの作成

		/*if (!CreateTextureImage(createInfo)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView(createInfo)) return false;// シェーダーで取り扱う用のImageViewを作成
		if (!CreateTextureSampler(createInfo)) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構*/

		if (!CreateDescriptorSetLayout(createInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
		if (!CreateUniformBuffers(createInfo)) return false; // ユニフォームバッファを作成
		if (!CreateDescriptorPool(createInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
		if (!CreateDescriptorSets(createInfo)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/

		return true;
	}

	bool CVulkanMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		// 共通のユニフォームバッファの更新
		SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
		SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);

		return true;
	}

	bool CVulkanMaterial::BuildDrawBuffer()
	{
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			const auto& BufferData = m_UniformBufferList[i]->GetData();
			auto bufferSize = BufferData.size();

			std::memcpy(m_VKUniformBufferMappedList[m_pGraphicsAPI->GetCurrentFrame()][i], &BufferData[0], bufferSize);
		}

		return true;
	}

	void CVulkanMaterial::SetUniformValue(const std::string Name, const void* Value)
	{
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			auto& UniformBuffer = m_UniformBufferList[i];
			const auto& UniformDesc = UniformBuffer->GetDescriptor();

			const auto& DataList = UniformDesc->GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int ByteOffset = UniformData->second.ByteOffset;
				const int ByteSize = UniformData->second.ByteSize;
				
				UniformBuffer->SetValue(Value, ByteOffset, ByteSize);
			}
		}
	}

	void CVulkanMaterial::Release()
	{
		// テクスチャサンプラーを破棄
		if (m_TextureSampler)
		{
			vkDestroySampler(m_pGraphicsAPI->GetLogicalDevice(), m_TextureSampler, nullptr);
			m_TextureSampler = nullptr;
		}


		// テクスチャ用のイメージビューの破棄
		if (m_TextureImageView)
		{
			vkDestroyImageView(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageView, nullptr);
			m_TextureImageView = nullptr;
		}


		// テクスチャイメージの破棄
		if (m_TextureImage)
		{
			vkDestroyImage(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImage, nullptr);
			m_TextureImage = nullptr;
		}

		if (m_TextureImageMemory)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageMemory, nullptr);
			m_TextureImageMemory = nullptr;
		}

		// ユニフォームの破棄
		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (auto& Buffer : m_VKUniformBufferList[i])
			{
				vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
			}
			
			for (auto& Memory : m_VKUniformBufferMemoryList[i])
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
		//if (createInfo.IsUseMainTexture())
		if (false)
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
		if (false)
		{
			m_TextureImageView = m_pGraphicsAPI->CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		return true;
	}

	bool CVulkanMaterial::CreateTextureSampler(const graphics::CMaterialCreateInfo& createInfo)
	{
		if (false)
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
		for (const auto& Buffer : m_UniformBufferList)
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
		m_VKUniformBufferList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferMemoryList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferMappedList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferSizeList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (const auto& Buffer : m_UniformBufferList)
			{
				const auto& Data = Buffer->GetData();
				size_t bufferSize = Data.size();

				VkBuffer UniformBuffer;
				VkDeviceMemory BufferMemory;
				void* BuffersMappedList;

				// バッファの作成
				m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);

				// バッファ用のメモリを作成
				vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), BufferMemory, 0, bufferSize, 0, &BuffersMappedList);

				// メモリに値を代入
				std::memcpy(BuffersMappedList, &Data[0], bufferSize);

				//
				m_VKUniformBufferList[i].push_back(UniformBuffer);
				m_VKUniformBufferMemoryList[i].push_back(BufferMemory);
				m_VKUniformBufferMappedList[i].push_back(BuffersMappedList);
				m_VKUniformBufferSizeList[i].push_back(bufferSize);
			}
		}

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorPool(const graphics::CMaterialCreateInfo& createInfo)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (const auto& Buffer : m_UniformBufferList)
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
		for (size_t FrameIndex = 0; FrameIndex < m_pGraphicsAPI->GetMaxFramesInFlight(); FrameIndex++)
		{
			for (int BufferIndex = 0; BufferIndex < m_UniformBufferList.size(); BufferIndex++)
			{
				const auto& Buffer = m_UniformBufferList[BufferIndex];
				size_t LayoutListSize = Buffer->GetBindingLayoutList().size();

				std::vector<VkWriteDescriptorSet> descriptorWrites(LayoutListSize);
				std::vector<VkDescriptorBufferInfo> bufferInfoList(LayoutListSize);
				std::vector<VkDescriptorImageInfo> imageInfoList(LayoutListSize);

				for (int LayoutIndex = 0; LayoutIndex < LayoutListSize; LayoutIndex++)
				{
					const auto& Layout = Buffer->GetBindingLayoutList()[LayoutIndex];

					descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrites[LayoutIndex].dstBinding = Layout.BindingIndex; // layout(location = n)
					descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???
					
					switch (Buffer->GetBufferType())
					{
					case graphics::EBufferType::UNIFROM:
						{
							bufferInfoList[LayoutIndex].buffer = m_VKUniformBufferList[FrameIndex][BufferIndex]; // UBOの指定
							bufferInfoList[LayoutIndex].offset = Layout.ByteOffset; // バッファオフセット
							bufferInfoList[LayoutIndex].range = Layout.ByteSize; // サイズかな？

							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // どのタイプのコマンドを発行してもらうのか
							descriptorWrites[LayoutIndex].descriptorCount = 1;
							descriptorWrites[LayoutIndex].pBufferInfo = &bufferInfoList[LayoutIndex];
						}
						break;

					case graphics::EBufferType::TEXTURE:
						{
							imageInfoList[LayoutIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							imageInfoList[LayoutIndex].imageView = m_TextureImageView;
							imageInfoList[LayoutIndex].sampler = m_TextureSampler;

							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							descriptorWrites[LayoutIndex].descriptorCount = 1;
							descriptorWrites[LayoutIndex].pImageInfo = &imageInfoList[LayoutIndex];
						}
						break;

					default:
						break;
					}
				}

				// たぶんバッファの転送を行うコマンドを発行している
				vkUpdateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}
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