#ifndef __DAWN__
#include "CVulkanRenderer.h"
#include "../GraphicsAPI/CVulkanAPI.h"
#include "CRendererCreateInfo.h"

namespace renderer
{
	CVulkanRenderer::CVulkanRenderer():
		m_pGraphicsAPI(nullptr),
		m_UseMainTexture(false),
		m_IndicesCount(0)
	{
	}

	CVulkanRenderer::~CVulkanRenderer()
	{
		Release();
	}

	void CVulkanRenderer::Release()
	{
		if (m_UseMainTexture)
		{
			// テクスチャサンプラーを破棄
			vkDestroySampler(m_pGraphicsAPI->GetLogicalDevice(), m_TextureSampler, nullptr);

			// テクスチャ用のイメージビューの破棄
			vkDestroyImageView(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageView, nullptr);

			// テクスチャイメージの破棄
			vkDestroyImage(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImage, nullptr);
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_TextureImageMemory, nullptr);
		}

		// ユニフォームの破棄
		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), m_UniformBuffers[i], nullptr);
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_UniformBuffersMemory[i], nullptr);
		}

		// 記述子プールの破棄
		vkDestroyDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorPool, nullptr);

		// ユニフォームレイアウトセットを破棄
		vkDestroyDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);

		// インデックスバッファの破棄
		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBuffer, nullptr);

		// インデックスバッファ用に確保したメモリ領域を破棄
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_IndexBufferMemory, nullptr);

		// 頂点バッファの破棄
		for (auto& Buffer : m_VertexBufferList)
		{
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
		}
		
		// 頂点バッファ用に確保したメモリ領域を破棄
		for (auto& Memory : m_VertexBufferMemoryList)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), Memory, nullptr);
		}

		// グラフィックパイプラインの破棄
		vkDestroyPipeline(m_pGraphicsAPI->GetLogicalDevice(), m_GraphicsPipeline, nullptr);

		// パイプラインレイアウトの破棄(たぶん本来は3Dオブジェクトごとにあるやつ) 
		vkDestroyPipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), m_PipelineLayout, nullptr);
	}

	bool CVulkanRenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);
		m_UseMainTexture = createInfo.IsUseMainTexture();

		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを作成
		if (!CreateIndexBuffer(createInfo)) return false; // インデックスバッファを作成

		/*if (!CreateTextureImage(createInfo)) return false; // テクスチャイメージの生成
		if (!CreateTextureImageView(createInfo)) return false;// シェーダーで取り扱う用のImageViewを作成
		if (!CreateTextureSampler(createInfo)) return false; // テクスチャサンプラーを作成.サンプラーとはテクスチャデータをフラグメント(3Dモデル)に合うように調整する機構*/

		if (!CreateDescriptorSetLayout(createInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
		if (!CreateUniformBuffers(createInfo)) return false; // ユニフォームバッファを作成
		if (!CreateDescriptorPool(createInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
		if (!CreateDescriptorSets(createInfo)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/

		if (!CreateGraphicsPipeline(createInfo)) return false; // グラフィックパイプラインを作成

		return true;
	}

	bool CVulkanRenderer::Update(float SecondsTime)
	{
		// ユニフォームデータの更新
		UpdateUniformBuffer(m_pGraphicsAPI->GetCurrentFrame(), SecondsTime);

		return true;
	}

	void CVulkanRenderer::UpdateUniformBuffer(uint32_t CurrentImage, float SecondsTime)
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
		std::memcpy(m_UniformBuffersMapped[CurrentImage], &Data[0], bufferSize);
	}

	bool CVulkanRenderer::Draw()
	{
		// グラフィックパイプラインをコマンドにバインド
		vkCmdBindPipeline(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// 頂点バッファをパイプラインにバインドする
		VkDeviceSize offsets[] = { 0 };
		for (int i = 0; i < static_cast<int>(m_VertexBufferList.size()); i++)
		{
			vkCmdBindVertexBuffers(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], i, 1, &m_VertexBufferList[i], offsets);
		}

		// インデックスバッファをパイプラインにバインドする
		vkCmdBindIndexBuffer(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

		// UBOのセット
		vkCmdBindDescriptorSets(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout, 0, 1, &m_DescriptorSets[m_pGraphicsAPI->GetCurrentFrame()], 0, nullptr);

		// 描画コマンドを発行
		//vkCmdDraw(m_CommandBuffers[m_CurrentFrame], 3, 1, 0, 0); // パラメーター: vertexCount, instanceCount, firstVertex, firstInstance
		// インデックス付のドローコマンドはこちら
		vkCmdDrawIndexed(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], m_IndicesCount, 1, 0, 0, 0);

		return true;
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanRenderer::CreateTextureImage(const CRendererCreateInfo& createInfo)
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

	bool CVulkanRenderer::CreateTextureImageView(const CRendererCreateInfo& createInfo)
	{
		if (createInfo.IsUseMainTexture())
		{
			m_TextureImageView = m_pGraphicsAPI->CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		return true;
	}

	bool CVulkanRenderer::CreateTextureSampler(const CRendererCreateInfo& createInfo)
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

	bool CVulkanRenderer::CreateVertexBuffer(const CRendererCreateInfo& createInfo)
	{
		// 頂点バッファオブジェクトの生成
		for (const auto& VertexData : createInfo.GetVertices())
		{
			//
			VkDeviceSize bufferSize = sizeof(VertexData[0]) * VertexData.size();

			// ステージングバッファの作成
			// ステージングバッファは頂点データ配列からデータをアップロードするのに使用するCPUアクセス可なバッファ
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				stagingBuffer, stagingBufferMemory);

			// 頂点データを渡すためのメモリのポインターを取得
			void* data;
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

			// 取得したポインタにデータをコピーする
			std::memcpy(data, VertexData.data(), (size_t)bufferSize);

			// マップを解除する。たぶんマップというのはCPUからGPUへデータを渡すために一時的に確保される入口みたいなものかな？
			// 渡し終わったのでポインタという名の通路・入口を破棄したみたいな
			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

			// 最終的に頂点バッファを保持するのに使用するバッファを作成
			VkBuffer Buffer;
			VkDeviceMemory BufferMemory;

			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				Buffer, BufferMemory);

			// バッファをコピー
			m_pGraphicsAPI->CopyBuffer(stagingBuffer, Buffer, bufferSize);

			// 不要なリソースを破棄
			vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

			// バッファを保存
			m_VertexBufferList.push_back(Buffer);
			m_VertexBufferMemoryList.push_back(BufferMemory);
		}

		return true;
	}
	bool CVulkanRenderer::CreateIndexBuffer(const CRendererCreateInfo& createInfo)
	{
		VkDeviceSize bufferSize = sizeof(createInfo.GetIndices()[0]) * createInfo.GetIndices().size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, createInfo.GetIndices().data(), (size_t)bufferSize);
		vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory);

		m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer, m_IndexBufferMemory);

		m_pGraphicsAPI->CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), stagingBufferMemory, nullptr);

		m_IndicesCount = static_cast<uint32_t>(createInfo.GetIndices().size());

		return true;
	}

	bool CVulkanRenderer::CreateDescriptorSetLayout(const CRendererCreateInfo& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		// レイアウトのバインドに関する設定
		// Model, View Proj等のUBOのレイアウト
		VkDescriptorSetLayoutBinding uboLayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
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
		}

		// レイアウトの作成に関する設定
		VkDescriptorSetLayoutCreateInfo layoutInfo{}; // : bindingをまとめるためのオブジェクト
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);

		return (result == VK_SUCCESS);
	}

	bool CVulkanRenderer::CreateUniformBuffers(const CRendererCreateInfo& createInfo)
	{
		VkDeviceSize bufferSize = sizeof(float) * 16 * 4 + sizeof(float) * 4 * 4;

		m_UniformBuffers.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_UniformBuffersMemory.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_UniformBuffersMapped.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_UniformBuffers[i], m_UniformBuffersMemory[i]);

			// 後で書き込むのでひとまず空でマップする
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
		}

		return true;
	}
	bool CVulkanRenderer::CreateDescriptorPool(const CRendererCreateInfo& createInfo)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		{
			VkDescriptorPoolSize poolSize{};

			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

			poolSizes.push_back(poolSize);
		}
		
		if(createInfo.IsUseMainTexture())
		{
			VkDescriptorPoolSize poolSize{};

			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
	bool CVulkanRenderer::CreateDescriptorSets(const CRendererCreateInfo& createInfo)
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
			// UBO用
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
			}
			
			vkUpdateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		return true;
	}

	bool CVulkanRenderer::CreateGraphicsPipeline(const CRendererCreateInfo& createInfo)
	{
		// シェーダーの準備
		auto VertexShadeCode = createInfo.GetVertexShaderCode();
		auto FragShadeCode = createInfo.GetFragmentShaderCode();

		// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
		// 使う時にGeometryとかTessellationも追加する
		VkShaderModule vertShaderModule;
		const auto& VertexShaderData = createInfo.GetVertexShaderCode();
		const bool UseVertexShader = CreateShaderModule(vertShaderModule, std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

		VkShaderModule fragShaderModule;
		const auto& FragmentShaderCode = createInfo.GetFragmentShaderCode();
		const bool UseFragmentShader = CreateShaderModule(fragShaderModule, std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));

		// シェーダーステージの作成(VertexShaderとかFragment, Geometryとかそういうステージ)
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		if (UseVertexShader)
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";

			shaderStages.push_back(vertShaderStageInfo);
		}

		if (UseFragmentShader)
		{
			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			shaderStages.push_back(fragShaderStageInfo);
		}

		// グラフィックパイプラインの固定機の設定 ///////////////////////////////////////////////////////////////////////////////////////

		// 動的状態(ダイナミックステート)の設定(パイプラインにベイクせずにマイフレームの描画時に設定できるようにするパラメーターの設定)
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		// 頂点バッファ入力(Vertex Shaderに渡すデータ形式について設定する)
		int Size = static_cast<int>(m_VertexBufferList.size());
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(Size);
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(Size);

		for (int i = 0; i < Size; i++)
		{
			//
			int Dimension = createInfo.GetAttributeDimensions()[i];

			// 頂点バッファのバインドに関する説明,設定(頂点バッファレイアウト)
			bindingDescriptions[i].binding = i; // バインドする頂点バッファのインデックス(?)違う形式で頂点バッファを用意するときに使用する？
			bindingDescriptions[i].stride = Dimension * sizeof(createInfo.GetVertices()[i][0]); // 頂点バッファ内の要素一つあたりのサイズ。次の要素までのバイト数
			bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // よくわからぬ。各頂点の後、次のデータ エントリに移動します。らしい

			// アトリビュート(頂点データ)の設定
			attributeDescriptions[i].binding = i; // BindingDescriptionの内どのバインド設定を使用するかのインデックス
			attributeDescriptions[i].location = i; // Shaderのlayout(location = 0)に設定すｒ数値
			attributeDescriptions[i].format = GetVertexFormat(Dimension); // データ型. SFLOAT --> Signed Float
			attributeDescriptions[i].offset = 0; // データオフセット
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInto{};
		vertexInputInto.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInto.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size()); // 頂点バッファのバインドに関するの設定
		vertexInputInto.pVertexBindingDescriptions = &bindingDescriptions[0];
		vertexInputInto.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());  // 頂点データ(アトリビュート)の設定
		vertexInputInto.pVertexAttributeDescriptions = &attributeDescriptions[0];

		// 入力アセンブリ(頂点から描画されるジオメトリの種類など, GL_TRIANGLE_STRIPみたいなのを設定する場所)
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // Trueにすると自分で定義したインデックスバッファが使用できる？

		// ビューポートの設定
		// 上記のダイナミックステートのことで動的変更を可にする
		VkViewport viewport{};
		viewport.x = 0.0f; // 基準の座標
		viewport.y = 0.0f;
		viewport.width = (float)m_pGraphicsAPI->GetSwapChainExtent().width;
		viewport.height = (float)m_pGraphicsAPI->GetSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// シザーの設定(シザーとはピクセルが実際に格納される領域を定義する. シザーよりも外側の領域はラスタライザにより破棄される)
		// 上記のダイナミックステートのことで動的変更を可にする
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_pGraphicsAPI->GetSwapChainExtent(); // 解像度

		// ビューポートとシザーの作成
		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = &viewport;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = &scissor;

		// ラスタライザの設定
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // カリングの設定
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // カリングする際の頂点の順番かな？ GL_CWWみたいな
		rasterizer.depthBiasEnable = VK_FALSE; // デプステストに関する設定
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		// マルチサンプリング(アンチエイリアシング)
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE; // アンチエイリアシングを無効にしておく
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;

		// デプステスト, ステンシルテスト 
		// ひとまず今は何もしない

		// カラーブレンディング /////////////////////////////////////////////
		// ローカルカラーブレンディング(アタッチされたフレームバッファごとの設定)
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// グローバルカラーブレンディング(全体で共通の設定???)
		VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
		colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo.logicOpEnable = VK_FALSE;
		colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingInfo.attachmentCount = 1;
		colorBlendingInfo.pAttachments = &colorBlendAttachment;
		colorBlendingInfo.blendConstants[0] = 0.0f;
		colorBlendingInfo.blendConstants[1] = 0.0f;
		colorBlendingInfo.blendConstants[2] = 0.0f;
		colorBlendingInfo.blendConstants[3] = 0.0f;

		///////////////////////////////////////////////////////////////////

		// パイプラインレイアウト(Uniformをシェーダーに渡すための仕組み)
		// Uniformの値自体はいつでも変更できるが、どのUniformを使用するかはここで事前にこのパイプラインレイアウトで設定しておく必要がある。
		// たぶんここではLayoutは意味としてUniformを指すのでは？
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) return false;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// レンダリングパイプラインでデプスとステンシルを有効にする
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		// これまでの情報をもとにレンダリングパイプラインを構築
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2; // しぇだーステージの数
		pipelineInfo.pStages = shaderStages.data();

		pipelineInfo.pVertexInputState = &vertexInputInto;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlendingInfo;
		pipelineInfo.pDynamicState = &dynamicStateCreateInfo;

		pipelineInfo.layout = m_PipelineLayout;

		pipelineInfo.renderPass = m_pGraphicsAPI->GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // パイプラインから新しいパイプラインを派生して作成するためのフィールド?
		pipelineInfo.basePipelineIndex = -1; // 今は何もしていない

		if (vkCreateGraphicsPipelines(m_pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) return false;

		// シェーダーモジュールを破棄する
		vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), fragShaderModule, nullptr);
		vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), vertShaderModule, nullptr);

		return true;
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader
	// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
	bool CVulkanRenderer::CreateShaderModule(VkShaderModule& shaderModule, const std::string& code)
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

	VkFormat CVulkanRenderer::GetVertexFormat(int Dimention)
	{
		VkFormat format = VK_FORMAT_UNDEFINED;

		switch (Dimention)
		{
		case 1:
			format = VK_FORMAT_R32_SFLOAT;
			break;

		case 2:
			format = VK_FORMAT_R32G32_SFLOAT;
			break;

		case 3:
			format = VK_FORMAT_R32G32B32_SFLOAT;
			break;

		case 4:
			format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			break;
		}

		return format;
	}
}

#endif