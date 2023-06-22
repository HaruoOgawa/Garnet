#ifndef __DAWN__
#include "CVulkanMaterial.h"
#include "CVulkanAPI.h"
#include "CVulkanTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"
#include "../../Math/CMath.h"

namespace api
{
	CVulkanMaterial::CVulkanMaterial(api::CVulkanAPI* pGraphicsAPI):
		CMaterial(),
		m_pGraphicsAPI(pGraphicsAPI),

		m_VertShaderModule(nullptr),
		m_FragShaderModule(nullptr),

		m_DescriptorSetLayout(nullptr),
		m_DescriptorPool(nullptr),

		m_EmptyTexture(nullptr)
	{
		m_EmptyTexture = std::make_shared<CVulkanTexture>(pGraphicsAPI, false);
		std::vector<unsigned char> emptyPixel = { 0, 0, 0, 0 };
		m_EmptyTexture->Create(emptyPixel, static_cast<int>(emptyPixel.size() * sizeof(unsigned char)));
	}

	CVulkanMaterial::~CVulkanMaterial()
	{
		Release();
	}

	bool CVulkanMaterial::Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList)
	{
		if (!CreateShaderStages(m_CreateInfo)) return false; // Shaderの作成

		// Uniform Buffer
		if (!CreateUniformBuffers(m_CreateInfo)) return false; // ユニフォームバッファを作成

		// バインドグループ(UniformとTextureで共通項)
		if (!CreateDescriptorSetLayout(m_CreateInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
		if (!CreateDescriptorPool(m_CreateInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
		if (!CreateDescriptorSets(m_CreateInfo, TextureList)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/

		// 生成処理が終わったので不要なリソースを解放する
		m_CreateInfo = nullptr;

		return true;
	}

	bool CVulkanMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		// 共通のユニフォームバッファの更新
		SetUniformValue("view", &Camera->GetViewMatrix()[0][0]);
		SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0]);
		SetUniformValue("lightDir", &DrawInfo->GetLightDir()[0]);
		SetUniformValue("lightColor", &DrawInfo->GetLightColor()[0]);
		SetUniformValue("cameraPos", &Camera->GetPos()[0]);
		SetUniformValue("time", &SecondsTime);
		
		return true;
	}

	bool CVulkanMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			auto ByteSize = m_VKUniformBufferSizeList[m_pGraphicsAPI->GetCurrentFrame()][i];
			auto ByteOffset = ((m_UseDynamicUniform)? (DynamicOffsetNum - 1) * ByteSize : 0);

			// バッファデータの更新
			void* BuffersMappedList;
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i], ByteOffset, ByteSize, 0, &BuffersMappedList);

			const auto& BufferData = m_UniformBufferList[i]->GetData();
			auto bufferSize = BufferData.size();

			std::memcpy(BuffersMappedList, &BufferData[0], bufferSize);

			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i]);
		}

		return true;
	}

	void CVulkanMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
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
		// ユニフォームの破棄
		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			if (m_VKUniformBufferList.size() > 0)
			{
				for (auto& Buffer : m_VKUniformBufferList[i])
				{
					if (Buffer)
					{
						vkDestroyBuffer(m_pGraphicsAPI->GetLogicalDevice(), Buffer, nullptr);
					}
				}
			}

			if (m_VKUniformBufferMemoryList.size() > 0)
			{
				for (auto& Memory : m_VKUniformBufferMemoryList[i])
				{
					if (Memory)
					{
						vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), Memory, nullptr);
					}
				}
			}
		}

		// 記述子プールの破棄
		if (m_DescriptorPool)
		{
			vkDestroyDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = nullptr;
		}

		// ユニフォームレイアウトセットを破棄
		if (m_DescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
			m_DescriptorSetLayout = nullptr;
		}
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanMaterial::CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		// シェーダーの準備
		// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
		// 使う時にGeometryとかTessellationも追加する
		const auto& VertexShaderData = createInfo->GetVertexShaderCode();
		const bool UseVertexShader = CreateShaderModule(m_VertShaderModule, std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

		const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
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

	bool CVulkanMaterial::CreateDescriptorSetLayout(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		
		// UBOのバインドに関する設定
		for (const auto& Buffer : m_UniformBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				VkDescriptorSetLayoutBinding LayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
				LayoutBinding.binding = Layout.BindingIndex; // バインディングインデックス
				
				if (m_UseDynamicUniform)
				{
					LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; // バッファタイプ
				}
				else
				{
					LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // バッファタイプ
				}
				
				LayoutBinding.descriptorCount = 1; // 
				LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
				LayoutBinding.pImmutableSamplers = nullptr; // 画像のサンプリングに使用するフィールド

				bindings.push_back(LayoutBinding);
			}
		}

		// テクスチャバインドに関する設定
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
			{
				VkDescriptorSetLayoutBinding LayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
				LayoutBinding.binding = TexLayout.ViewBindingIndex; // バインディングインデックス

				LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; // バッファタイプ

				LayoutBinding.descriptorCount = 1; // 
				LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
				LayoutBinding.pImmutableSamplers = nullptr; // 画像のサンプリングに使用するフィールド

				bindings.push_back(LayoutBinding);
			}

			{
				VkDescriptorSetLayoutBinding LayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
				LayoutBinding.binding = TexLayout.SamplerBindingIndex; // バインディングインデックス

				LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER; // バッファタイプ

				LayoutBinding.descriptorCount = 1; // 
				LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
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

	bool CVulkanMaterial::CreateUniformBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		m_VKUniformBufferList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferMemoryList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferSizeList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (const auto& Buffer : m_UniformBufferList)
			{
				const auto& Data = Buffer->GetData();
				const uint32_t ByteSize = static_cast<uint32_t>(math::GetNextPowerOfTwo(static_cast<unsigned int>(Data.size()))); // 2のn乗にする

				VkBuffer UniformBuffer;
				VkDeviceMemory BufferMemory;

				// バッファの作成
				if (m_UseDynamicUniform)
				{
					m_pGraphicsAPI->CreateBuffer(ByteSize * m_RefCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, UniformBuffer, BufferMemory);
				}
				else
				{
					m_pGraphicsAPI->CreateBuffer(ByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);
				}

				//
				m_VKUniformBufferList[i].push_back(UniformBuffer);
				m_VKUniformBufferMemoryList[i].push_back(BufferMemory);
				m_VKUniformBufferSizeList[i].push_back(ByteSize);
			}
		}

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorPool(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		// UBOのプール
		for (const auto& Buffer : m_UniformBufferList)
		{
			VkDescriptorPoolSize poolSize{};

			if (m_UseDynamicUniform)
			{
				poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			}
			else
			{
				poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}

			poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

			poolSizes.push_back(poolSize);
		}

		// テクスチャのプール
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

				poolSizes.push_back(poolSize);
			}

			{
				VkDescriptorPoolSize poolSize{};
				poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
				poolSize.descriptorCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

				poolSizes.push_back(poolSize);
			}
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
	bool CVulkanMaterial::CreateDescriptorSets(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList)
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
			//
			for (int BufferIndex = 0; BufferIndex < m_UniformBufferList.size(); BufferIndex++)
			{
				const auto& Buffer = m_UniformBufferList[BufferIndex];
				size_t UniformLayoutSize = Buffer->GetBindingLayoutList().size();
				size_t TexLayoutSize = m_TextureBindingLayoutList.size() * 2; // ImageViewとSamplerがあるので2倍にしている

				std::vector<VkWriteDescriptorSet> descriptorWrites(UniformLayoutSize + TexLayoutSize);
				std::vector<VkDescriptorBufferInfo> bufferInfoList(UniformLayoutSize);
				std::vector<VkDescriptorImageInfo> imageInfoList(TexLayoutSize);
				
				int LayoutIndex = 0;

				// UBO
				for (int BufferLayoutIndex = 0; BufferLayoutIndex < UniformLayoutSize; BufferLayoutIndex++)
				{
					auto& Layout = Buffer->GetBindingLayoutList()[BufferLayoutIndex];
					descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrites[LayoutIndex].dstBinding = Layout.BindingIndex; // layout(location = n)
					descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???
					
					bufferInfoList[BufferLayoutIndex].buffer = m_VKUniformBufferList[FrameIndex][BufferIndex]; // UBOの指定
					bufferInfoList[BufferLayoutIndex].offset = Layout.ByteOffset; // バッファオフセット
					bufferInfoList[BufferLayoutIndex].range = Layout.ByteSize; // サイズかな？

					if (m_UseDynamicUniform)
					{
						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; // どのタイプのコマンドを発行してもらうのか
					}
					else
					{
						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // どのタイプのコマンドを発行してもらうのか
					}

					descriptorWrites[LayoutIndex].descriptorCount = 1;
					descriptorWrites[LayoutIndex].pBufferInfo = &bufferInfoList[BufferLayoutIndex];

					// 複数個入力しても意味がないので始めのFrameIndexだけを見る
					if (FrameIndex == 0)
					{
						m_BindingRefSizeList.push_back(m_VKUniformBufferSizeList[FrameIndex][BufferIndex]);
					}

					LayoutIndex++;
				}

				// テクスチャ
				for (int ImageInfoIndex = 0, TextureBindingLayoutIndex = 0; ImageInfoIndex < TexLayoutSize; ImageInfoIndex += 2, TextureBindingLayoutIndex++)
				{
					const auto& TexLayout = m_TextureBindingLayoutList[TextureBindingLayoutIndex];
					const auto& Texture = (TexLayout.TextureIndex >= 0) ? static_cast<api::CVulkanTexture*>(TextureList[TexLayout.TextureIndex].get()) : m_EmptyTexture.get();

					{
						descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
						descriptorWrites[LayoutIndex].dstBinding = TexLayout.ViewBindingIndex; // layout(location = n)
						descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???

						imageInfoList[ImageInfoIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfoList[ImageInfoIndex].imageView = Texture->GetTextureImageView();

						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
						descriptorWrites[LayoutIndex].descriptorCount = 1;
						descriptorWrites[LayoutIndex].pImageInfo = &imageInfoList[ImageInfoIndex];

						LayoutIndex++;
					}

					{
						descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
						descriptorWrites[LayoutIndex].dstBinding = TexLayout.SamplerBindingIndex; // layout(location = n)
						descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???

						imageInfoList[ImageInfoIndex + 1].sampler = Texture->GetTextureSampler();

						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
						descriptorWrites[LayoutIndex].descriptorCount = 1;
						descriptorWrites[LayoutIndex].pImageInfo = &imageInfoList[ImageInfoIndex + 1];

						LayoutIndex++;
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