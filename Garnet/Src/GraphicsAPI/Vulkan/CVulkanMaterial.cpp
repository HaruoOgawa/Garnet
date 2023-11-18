#ifdef USE_VULKAN
#include "CVulkanMaterial.h"
#include "CVulkanAPI.h"
#include "CVulkanTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"

namespace api
{
	CVulkanMaterial::CVulkanMaterial(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo):
		CMaterial(createInfo),
		m_pGraphicsAPI(pGraphicsAPI),

		m_VertShaderModule(nullptr),
		m_FragShaderModule(nullptr),
		m_ComputeShaderModule(nullptr),

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

	bool CVulkanMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		if (!CreateShaderStages(m_CreateInfo)) return false; // Shaderの作成

		// Uniform Buffer
		if (!CreateShaderBuffers(m_CreateInfo)) return false; // ユニフォームバッファを作成

		// バインドグループ(UniformとTextureで共通項)
		if (!CreateDescriptorSetLayout(m_CreateInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
		if (!CreateDescriptorPool(m_CreateInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
		if (!CreateDescriptorSets(m_CreateInfo, TextureSet)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/

		// 生成処理が終わったので不要なリソースを解放する
		m_CreateInfo = nullptr;

		return true;
	}

	bool CVulkanMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			if (m_ShaderBufferList[i]->GetBufferUpdateType() != graphics::EBufferUpdateType::UPDATE_TYPE_CPU) continue;

			auto ByteSize = m_VKUniformBufferSizeList[m_pGraphicsAPI->GetCurrentFrame()][i];
			auto ByteOffset = ((m_UseDynamicBufferOffset)? (DynamicOffsetNum - 1) * ByteSize : 0);

			// バッファデータの更新
			void* BuffersMappedList;
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i], ByteOffset, ByteSize, 0, &BuffersMappedList);

			const auto& BufferData = m_ShaderBufferList[i]->GetData();
			auto bufferSize = BufferData.size();

			std::memcpy(BuffersMappedList, &BufferData[0], bufferSize);

			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i]);
		}

		return true;
	}

	void CVulkanMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			auto& UniformBuffer = m_ShaderBufferList[i];
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
		// ShaderModuleの破棄
		if (m_VertShaderModule)
		{
			vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), m_VertShaderModule, nullptr);
			m_VertShaderModule = nullptr;
		}
		
		if (m_FragShaderModule)
		{
			vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), m_FragShaderModule, nullptr);
			m_FragShaderModule = nullptr;
		}
		
		if (m_ComputeShaderModule)
		{
			vkDestroyShaderModule(m_pGraphicsAPI->GetLogicalDevice(), m_ComputeShaderModule, nullptr);
			m_ComputeShaderModule = nullptr;
		}

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
		bool UseVertexShader = false;
		if (!VertexShaderData.empty()) UseVertexShader = CreateShaderModule(m_VertShaderModule, std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

		const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
		bool UseFragmentShader = false;
		if(!FragmentShaderCode.empty()) UseFragmentShader = CreateShaderModule(m_FragShaderModule, std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));

		const auto& ComputeShaderCode = createInfo->GetComputeShaderCode();
		bool UseComputeShader = false;
		if(!ComputeShaderCode.empty()) UseComputeShader = CreateShaderModule(m_ComputeShaderModule, std::string(&ComputeShaderCode[0], &ComputeShaderCode[0] + ComputeShaderCode.size()));

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

		if (UseComputeShader)
		{
			VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
			computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			computeShaderStageInfo.module = m_ComputeShaderModule;
			computeShaderStageInfo.pName = "main";

			m_ShaderStages.clear(); // ComputeShaderは単体でしか使わないので念のためリセットしておく

			m_ShaderStages.push_back(computeShaderStageInfo);
		}

		return true;
	}

	bool CVulkanMaterial::CreateDescriptorSetLayout(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		
		// UBOのバインドに関する設定
		for (const auto& Buffer : m_ShaderBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				VkDescriptorSetLayoutBinding LayoutBinding{}; // VkDescriptorSetLayoutBindingはおそらくlayout(location = 0), WebGPUでいう @binding(n)のこと. ただしVulkanは @groupは存在しない
				LayoutBinding.binding = Layout.second.BindingIndex; // バインディングインデックス
				
				if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
				{
					if (m_UseDynamicBufferOffset)
					{
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; // バッファタイプ
					}
					else
					{
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // バッファタイプ
					}
				}
				else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
				{
					if (m_UseDynamicBufferOffset)
					{
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; // バッファタイプ
					}
					else
					{
						LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // バッファタイプ
					}
				}

				LayoutBinding.descriptorCount = 1; // 
				LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT; // アクセス権限。ここでは頂点シェーダーのみ読み取り可
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

	bool CVulkanMaterial::CreateShaderBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		m_VKUniformBufferList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferMemoryList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());
		m_VKUniformBufferSizeList.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		for (size_t i = 0; i < m_pGraphicsAPI->GetMaxFramesInFlight(); i++)
		{
			for (const auto& Buffer : m_ShaderBufferList)
			{
				// SharedBufferは処理しない
				if (Buffer->GetSharedBufferParam().IsShared) continue;

				const auto& Data = Buffer->GetData();
				const uint64_t ByteSize = static_cast<uint64_t>(Data.size());

				VkBuffer UniformBuffer = nullptr;
				VkDeviceMemory BufferMemory = nullptr;

				// バッファの作成
				if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
				{
					if (m_UseDynamicBufferOffset)
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize * m_RefCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, UniformBuffer, BufferMemory);
					}
					else
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);
					}
				}
				else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
				{
					if (m_UseDynamicBufferOffset)
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize * m_RefCount, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, UniformBuffer, BufferMemory);
					}
					else
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);
					}
				}

				// バッファの初期化
				if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
				{
					// ひとまずDynamicOffset周りでいろいろ問題が起こりそうなのでSSBOの時だけ初期化する
					// (様子をみて変えるかも)
					void* BuffersMappedList;
					vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), BufferMemory, 0, ByteSize, 0, &BuffersMappedList);
					std::memcpy(BuffersMappedList, &Data[0], ByteSize);
					vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), BufferMemory);
				}

				// バッファをリストに追加
				m_VKUniformBufferList[i].push_back(UniformBuffer);
				m_VKUniformBufferMemoryList[i].push_back(BufferMemory);
				m_VKUniformBufferSizeList[i].push_back(static_cast<uint32_t>(ByteSize));
			}
		}

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorPool(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		// UBOのプール
		for (const auto& Buffer : m_ShaderBufferList)
		{
			// SharedBufferは処理しない
			if (Buffer->GetSharedBufferParam().IsShared) continue;

			VkDescriptorPoolSize poolSize{};

			if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
			{
				if (m_UseDynamicBufferOffset)
				{
					poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				}
				else
				{
					poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
			}
			else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
			{
				if (m_UseDynamicBufferOffset)
				{
					poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
				}
				else
				{
					poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				}
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
	bool CVulkanMaterial::CreateDescriptorSets(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		//
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList(0);
		if (TextureSet) TextureList = TextureSet->Get2DTextureList();

		std::vector<std::shared_ptr<graphics::CTexture>> CubeMapList(0);
		if (TextureSet) CubeMapList = TextureSet->GetCubeMapList();

		std::vector<std::shared_ptr<graphics::CTexture>> FrameTextureList(0);
		if (TextureSet) FrameTextureList = TextureSet->GetFrameTextureList();

		std::shared_ptr<graphics::CTexture> Diffuse_Tex = nullptr;
		if (TextureSet) Diffuse_Tex = TextureSet->GetDiffuse_Tex();

		std::shared_ptr<graphics::CTexture> Specular_Tex = nullptr;
		if (TextureSet) Specular_Tex = TextureSet->GetSpecular_Tex();

		std::shared_ptr<graphics::CTexture> GGXLUT_Tex = nullptr;
		if (TextureSet) GGXLUT_Tex = TextureSet->GetGGXLUT_Tex();

		//
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
			for (int BufferIndex = 0; BufferIndex < m_ShaderBufferList.size(); BufferIndex++)
			{
				const auto& Buffer = m_ShaderBufferList[BufferIndex];
				size_t UniformLayoutSize = Buffer->GetBindingLayoutList().size();
				size_t TexLayoutSize = m_TextureBindingLayoutList.size() * 2; // ImageViewとSamplerがあるので2倍にしている

				std::vector<VkWriteDescriptorSet> descriptorWrites(UniformLayoutSize + TexLayoutSize);
				std::vector<VkDescriptorBufferInfo> bufferInfoList(UniformLayoutSize);
				std::vector<VkDescriptorImageInfo> imageInfoList(TexLayoutSize);
				
				int LayoutIndex = 0;

				// 共有バッファ
				const auto& SharedBufferParam = m_ShaderBufferList[BufferIndex]->GetSharedBufferParam();

				// UBO
				int BufferLayoutIndex = 0;
				for (const auto& Layout : Buffer->GetBindingLayoutList())
				{
					descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrites[LayoutIndex].dstBinding = Layout.second.BindingIndex; // layout(location = n)
					descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???

					if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
					{
						CVulkanMaterial* pSharedVulkanMat = static_cast<CVulkanMaterial*>(SharedBufferParam.SharedBufferMaterial.get());

						bufferInfoList[BufferLayoutIndex].buffer = pSharedVulkanMat->GetVKUniformBufferList()[FrameIndex][SharedBufferParam.BufferIndex]; // UBOの指定
					}
					else // 通常のバッファ使用
					{
						bufferInfoList[BufferLayoutIndex].buffer = m_VKUniformBufferList[FrameIndex][BufferIndex]; // UBOの指定
					}

					bufferInfoList[BufferLayoutIndex].offset = Layout.second.ByteOffset; // バッファオフセット
					bufferInfoList[BufferLayoutIndex].range = Layout.second.ByteSize; // サイズかな？

					if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
					{
						if (m_UseDynamicBufferOffset)
						{
							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC; // どのタイプのコマンドを発行してもらうのか
						}
						else
						{
							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // どのタイプのコマンドを発行してもらうのか
						}
					}
					else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
					{
						if (m_UseDynamicBufferOffset)
						{
							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; // どのタイプのコマンドを発行してもらうのか
						}
						else
						{
							descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // どのタイプのコマンドを発行してもらうのか
						}
					}

					descriptorWrites[LayoutIndex].descriptorCount = 1;
					descriptorWrites[LayoutIndex].pBufferInfo = &bufferInfoList[BufferLayoutIndex];

					// 複数個入力しても意味がないので始めのFrameIndexだけを見る
					if (FrameIndex == 0)
					{
						if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
						{
							CVulkanMaterial* pSharedVulkanMat = static_cast<CVulkanMaterial*>(SharedBufferParam.SharedBufferMaterial.get());

							m_BindingRefSizeList.push_back(pSharedVulkanMat->GetVKUniformBufferSizeList()[FrameIndex][BufferIndex]);
						}
						else
						{
							m_BindingRefSizeList.push_back(m_VKUniformBufferSizeList[FrameIndex][BufferIndex]);
						}
					}

					BufferLayoutIndex++;
					LayoutIndex++;
				}

				// テクスチャ
				for (int ImageInfoIndex = 0, TextureBindingLayoutIndex = 0; ImageInfoIndex < TexLayoutSize; ImageInfoIndex += 2, TextureBindingLayoutIndex++)
				{
					const auto& TexLayout = m_TextureBindingLayoutList[TextureBindingLayoutIndex];

					api::CVulkanTexture* Texture = nullptr;
					int TextureIndex = TexLayout.TextureIndex;

					if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
					{
						Texture = (TextureIndex >= 0 && TextureIndex < TextureList.size()) ? static_cast<api::CVulkanTexture*>(TextureList[TextureIndex].get()) : m_EmptyTexture.get();
					}
					else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
					{
						Texture = (TextureIndex >= 0 && TextureIndex < CubeMapList.size()) ? static_cast<api::CVulkanTexture*>(CubeMapList[TextureIndex].get()) : m_EmptyTexture.get();
					}
					else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
					{
						Texture = (TextureIndex >= 0 && TextureIndex < FrameTextureList.size()) ? static_cast<api::CVulkanTexture*>(FrameTextureList[TextureIndex].get()) : m_EmptyTexture.get();
					}
					else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse)
					{
						Texture = (TextureIndex >= 0 && Diffuse_Tex) ? static_cast<api::CVulkanTexture*>(Diffuse_Tex.get()) : m_EmptyTexture.get();
					}
					else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular)
					{
						Texture = (TextureIndex >= 0 && Specular_Tex) ? static_cast<api::CVulkanTexture*>(Specular_Tex.get()) : m_EmptyTexture.get();
					}
					else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT)
					{
						Texture = (TextureIndex >= 0 && GGXLUT_Tex) ? static_cast<api::CVulkanTexture*>(GGXLUT_Tex.get()) : m_EmptyTexture.get();
					}

					if (!Texture)
					{
						Console::Log("[ERROR] Texture is nullpte\n");
						return false;
					}

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
		if (code.empty()) return false;

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