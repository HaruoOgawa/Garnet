#ifdef USE_VULKAN
#include "CVulkanMaterial.h"
#include "CVulkanAPI.h"
#include "CVulkanTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CTransform.h"

namespace api
{
	CVulkanMaterial::CVulkanMaterial(api::CVulkanAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode):
		CMaterial(pGraphicsAPI, createInfo, CullMode),
		m_pGraphicsAPI(pGraphicsAPI),

		m_VertShaderModule(nullptr),
		m_FragShaderModule(nullptr),
		m_ComputeShaderModule(nullptr),

		m_DescriptorSetLayout(nullptr),
		m_DescriptorPool(nullptr),

		m_PipelineLayout(nullptr)
	{
	}

	CVulkanMaterial::~CVulkanMaterial()
	{
		Release();
	}

	void CVulkanMaterial::Release()
	{
		//
		m_ShaderStages.clear();

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

		// ShaderObjectの削除
		if (m_pGraphicsAPI->IsEnabledRuntimeShaderEditing())
		{

			for (auto& Shader : m_ShaderMap)
			{
				m_pGraphicsAPI->DestroyShaderEXT(m_pGraphicsAPI->GetLogicalDevice(), Shader.second, nullptr);
			}

			m_ShaderMap.clear();
		}

		// パイプラインレイアウトの破棄(たぶん本来は3Dオブジェクトごとにあるやつ) 
		if (m_PipelineLayout)
		{
			vkDestroyPipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), m_PipelineLayout, nullptr);
			m_PipelineLayout = nullptr;
		}
		
		// 記述子プールの破棄
		if (m_DescriptorPool)
		{
			vkFreeDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorPool, static_cast<uint32_t>(m_DescriptorSets.size()), &m_DescriptorSets[0]);

			vkDestroyDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = nullptr;
		}

		// DescriptorSetsの破棄
		m_DescriptorSets.clear();

		// ユニフォームレイアウトセットを破棄
		if (m_DescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), m_DescriptorSetLayout, nullptr);
			m_DescriptorSetLayout = nullptr;
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

		m_VKUniformBufferList.clear();
		m_VKUniformBufferMemoryList.clear();
		m_VKUniformBufferSizeList.clear();
	}

	bool CVulkanMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		// 参照テクスチャリスト
		if (!CreateRefTextureList(m_CreateInfo, TextureSet)) return false;

		{
			// Uniform Buffer
			if (!CreateShaderBuffers(m_CreateInfo)) return false; // ユニフォームバッファを作成

			// バインドグループ(UniformとTextureで共通項)
			if (!CreateDescriptorSetLayout(m_CreateInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
			if (!CreateDescriptorPool(m_CreateInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
			if (!CreateDescriptorSets(m_CreateInfo)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/
			if (!CreatePipelineLayout()) return false; // パイプラインレイアウトを生成

			if (m_pGraphicsAPI->IsEnabledRuntimeShaderEditing())
			{
				// ShaderObjectの作成
				if (!CreateShaderObjects(m_CreateInfo)) return false;
			}
			else
			{
				if (!CreateShaderStages(m_CreateInfo)) return false; // Shaderの作成
			}
		}

		return true;
	}

	bool CVulkanMaterial::ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList)
	{
		m_CreateInfo = createInfo;

		// ShaderObjectの削除
		/*if (m_pGraphicsAPI->IsEnabledRuntimeShaderEditing())
		{

			for (auto& Shader : m_ShaderMap)
			{
				m_pGraphicsAPI->DestroyShaderEXT(m_pGraphicsAPI->GetLogicalDevice(), Shader.second, nullptr);
			}

			m_ShaderMap.clear();
		}*/

		// CommandのSubmit時にエラーが発生するので、ひとまずVulkanについてはShaderの更新のみとする
		// Uniformを編集したいときはOpenGLを使用する
		Release();

		// 参照テクスチャリストの再生成
		if (!ReCreateRefTextureList(m_CreateInfo)) return false;

		// バッファの再生成
		if (!ReCreateBuffer(ShaderBufferList, TextureBindingLayoutList)) return false;

		{
			// Uniform Buffer
			if (!CreateShaderBuffers(m_CreateInfo)) return false; // ユニフォームバッファを作成

			// バインドグループ(UniformとTextureで共通項)
			if (!CreateDescriptorSetLayout(m_CreateInfo)) return false; // DescriptorSetLayoutの作成(Uniformをどのようにバインドするか), WebGPUでいうバインドグループの生成
			if (!CreateDescriptorPool(m_CreateInfo)) return false; // DescriptorPoolを作成する -> DescriptorSetsは直接生成できず、コマンドで生成する必要がある。記述子プールはそのコマンド群のことかな？
			if (!CreateDescriptorSets(m_CreateInfo)) return false; // DescriptorSetsを作成 -> Uniformが使用するバッファをCPUからGPUに送信するための仕組みこと. https://vkguide.dev/docs/chapter-4/descriptors/
			if (!CreatePipelineLayout()) return false; // パイプラインレイアウトを生成

			if (m_pGraphicsAPI->IsEnabledRuntimeShaderEditing())
			{
				// ShaderObjectの作成
				if (!CreateShaderObjects(m_CreateInfo)) return false;
			}
			else
			{
				if (!CreateShaderStages(m_CreateInfo)) return false; // Shaderの作成
			}
		}

		return true;
	}

	bool CVulkanMaterial::BuildDrawBuffer()
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			if (m_ShaderBufferList[i]->GetBufferUpdateType() != graphics::EBufferUpdateType::UPDATE_TYPE_CPU) continue;

			auto ByteSize = m_VKUniformBufferSizeList[m_pGraphicsAPI->GetCurrentFrame()][i];
			int ByteOffset = 0;
			if (IsUseDynamicOffset())
			{
				const auto it = m_PassNameDynamicOffsetMap.find(m_pGraphicsAPI->GetCurrentRenderPassName());
				if (it == m_PassNameDynamicOffsetMap.end()) return false;

				ByteOffset = it->second * ByteSize;
			}

			// バッファデータの更新
			void* BuffersMappedList;
			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i], ByteOffset, ByteSize, 0, &BuffersMappedList);

			const auto& BufferData = m_ShaderBufferList[i]->GetBuffer();
			auto bufferSize = BufferData.size();

			std::memcpy(BuffersMappedList, &BufferData[0], bufferSize);

			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), m_VKUniformBufferMemoryList[m_pGraphicsAPI->GetCurrentFrame()][i]);
		}

		return true;
	}

	void CVulkanMaterial::SetUniformValue(const std::string Name, const void* Data, int ByteSize)
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			auto& UniformBuffer = m_ShaderBufferList[i];
			const auto& UniformDesc = UniformBuffer->GetDescriptor();

			const auto& DataList = UniformDesc.GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int ByteOffset = UniformData->second.ByteOffset;
				
				UniformBuffer->SetValue(Data, ByteOffset, ByteSize);
			}
		}
	}

	bool CVulkanMaterial::IsAvailable() const
	{
		if (m_ShaderStages.empty() && m_ShaderMap.empty()) return false;

		if (m_PipelineLayout == nullptr) return false;
		if (m_DescriptorSetLayout == nullptr) return false;
		if (m_DescriptorPool == nullptr) return false;

		if (m_DescriptorSets.empty()) return false;

		return true;
	}

	void CVulkanMaterial::SetActive()
	{
		for (const auto& Shader : m_ShaderMap)
		{
			m_pGraphicsAPI->BindShadersEXT(m_pGraphicsAPI->GetCurrentCommandBuffer(), 1, &Shader.first, &Shader.second);
		}
	}

	void CVulkanMaterial::BindUBO()
	{
		if (IsUseShaderBuffer())
		{
			std::vector<uint32_t> dynamicOffsetList;
			for (const auto& Size : GetBindingRefSizeList())
			{
				uint32_t dynamicOffset = 0;

				if (IsUseDynamicOffset())
				{
					const auto it = m_PassNameDynamicOffsetMap.find(m_pGraphicsAPI->GetCurrentRenderPassName());
					if (it == m_PassNameDynamicOffsetMap.end()) return;

					dynamicOffset = it->second * Size;
				}

				dynamicOffsetList.push_back(dynamicOffset);
			}

			// ShaderBufferを持っていないとdynamicOffsetListが空になることがあるが、これはDynamicOffsetが不要ということである
			if (IsUseDynamicOffset() && !dynamicOffsetList.empty())
			{
				vkCmdBindDescriptorSets(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					GetPipelineLayout(), 0, 1, &GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], static_cast<uint32_t>(dynamicOffsetList.size()), &dynamicOffsetList[0]);
			}
			else
			{
				vkCmdBindDescriptorSets(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					GetPipelineLayout(), 0, 1, &GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], 0, nullptr);
			}
		}
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanMaterial::CreateShaderObjects(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		std::vector<VkShaderCreateInfoEXT> CreateInfoList;

		// ひとまずVertexとFragmentのみ並列で作る
		{
			const auto& ShaderCode = createInfo->GetVertexShaderCode();

			VkShaderCreateInfoEXT shaderCreateInfo{};
			shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
			shaderCreateInfo.pNext = nullptr;
			shaderCreateInfo.flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;
			shaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderCreateInfo.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
			shaderCreateInfo.codeSize = ShaderCode.size();
			shaderCreateInfo.pCode = &ShaderCode[0];
			shaderCreateInfo.pName = "main";
			// RenderのGraphicsPipelineに渡していたDescriptorSetLayoutをここで渡せるので完全に切り離せそう？
			// つまりShaderBufferの更新もマテリアルだけで完結できそう？
			shaderCreateInfo.setLayoutCount = 1;
			shaderCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
			shaderCreateInfo.pushConstantRangeCount = 0;
			shaderCreateInfo.pPushConstantRanges = nullptr;
			shaderCreateInfo.pSpecializationInfo = nullptr;

			CreateInfoList.push_back(shaderCreateInfo);
		}

		{
			const auto& ShaderCode = createInfo->GetFragmentShaderCode();

			VkShaderCreateInfoEXT shaderCreateInfo{};
			shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
			shaderCreateInfo.pNext = nullptr;
			shaderCreateInfo.flags = VK_SHADER_CREATE_LINK_STAGE_BIT_EXT;
			shaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderCreateInfo.nextStage = 0;
			shaderCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
			shaderCreateInfo.codeSize = ShaderCode.size();
			shaderCreateInfo.pCode = &ShaderCode[0];
			shaderCreateInfo.pName = "main";
			// RenderのGraphicsPipelineに渡していたDescriptorSetLayoutをここで渡せるので完全に切り離せそう？
			// つまりShaderBufferの更新もマテリアルだけで完結できそう？
			shaderCreateInfo.setLayoutCount = 1;
			shaderCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
			shaderCreateInfo.pushConstantRangeCount = 0;
			shaderCreateInfo.pPushConstantRanges = nullptr;
			shaderCreateInfo.pSpecializationInfo = nullptr;

			CreateInfoList.push_back(shaderCreateInfo);
		}

		std::vector<VkShaderEXT> Shaders;
		Shaders.resize(static_cast<int>(CreateInfoList.size()));

		VK_CHECK_RESULT(m_pGraphicsAPI->CreateShadersEXT(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(CreateInfoList.size()), &CreateInfoList[0], nullptr, &Shaders[0]));

		for (int i = 0; i < static_cast<int>(CreateInfoList.size()); i++)
		{
			const auto& createInfo = CreateInfoList[i];

			m_ShaderMap.emplace(createInfo.stage, Shaders[i]);
		}

		return true;
	}
	
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
					if (IsUseDynamicOffset())
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
					if (IsUseDynamicOffset())
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

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_pGraphicsAPI->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout));

		return true;
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

				const auto& Data = Buffer->GetBuffer();
				const uint64_t ByteSize = static_cast<uint64_t>(Data.size());

				// DynamicOffsetはバッファサイズが256バイト以上でないと使用できないので使用する設定になっていてそれよりも小さい時はエラーとする
				if (IsUseDynamicOffset() && ByteSize < 256)
				{
					Console::Log("[API Error] ByteSize must be rather than 256 byte if use DynamicOffset. / BufferName: %s / ByteSize: %d\n", Buffer->GetBufferName().c_str(), static_cast<int>(ByteSize));

					return false;
				}

				VkBuffer UniformBuffer = nullptr;
				VkDeviceMemory BufferMemory = nullptr;

				const int RefCount = static_cast<int>(m_PassNameDynamicOffsetMap.size());

				// バッファの作成
				if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
				{
					if (IsUseDynamicOffset())
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize * RefCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, UniformBuffer, BufferMemory);
					}
					else
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, UniformBuffer, BufferMemory);
					}
				}
				else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
				{
					if (IsUseDynamicOffset())
					{
						m_pGraphicsAPI->CreateBuffer(ByteSize * RefCount, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, UniformBuffer, BufferMemory);
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
				if (IsUseDynamicOffset())
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
				if (IsUseDynamicOffset())
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
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());

		VK_CHECK_RESULT(vkCreateDescriptorPool(m_pGraphicsAPI->GetLogicalDevice(), &poolInfo, nullptr, &m_DescriptorPool));

		return true;
	}
	bool CVulkanMaterial::CreateDescriptorSets(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayout> layouts(m_pGraphicsAPI->GetMaxFramesInFlight(), m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_pGraphicsAPI->GetMaxFramesInFlight());
		allocInfo.pSetLayouts = layouts.data();

		//
		m_DescriptorSets.resize(m_pGraphicsAPI->GetMaxFramesInFlight());

		VK_CHECK_RESULT(vkAllocateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), &allocInfo, m_DescriptorSets.data()));

		//
		for (size_t FrameIndex = 0; FrameIndex < m_pGraphicsAPI->GetMaxFramesInFlight(); FrameIndex++)
		{
			// ShaderBuffer
			if (!UpdateShaderBufferDescriptorSets(FrameIndex)) return false;

			// テクスチャ
			if (!UpdateTextureBufferDescriptorSets(FrameIndex)) return false;
		}

		return true;
	}

	bool CVulkanMaterial::UpdateShaderBufferDescriptorSets(size_t FrameIndex)
	{
		for (int BufferIndex = 0; BufferIndex < m_ShaderBufferList.size(); BufferIndex++)
		{
			const auto& Buffer = m_ShaderBufferList[BufferIndex];
			size_t UniformLayoutSize = Buffer->GetBindingLayoutList().size();

			std::vector<VkWriteDescriptorSet> descriptorWrites(UniformLayoutSize);
			std::vector<VkDescriptorBufferInfo> bufferInfoList(UniformLayoutSize);

			int LayoutIndex = 0;

			// 共有バッファ
			const auto& SharedBufferParam = m_ShaderBufferList[BufferIndex]->GetSharedBufferParam();

			// UBO
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				descriptorWrites[LayoutIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[LayoutIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
				descriptorWrites[LayoutIndex].dstBinding = Layout.second.BindingIndex; // layout(location = n)
				descriptorWrites[LayoutIndex].dstArrayElement = 0; // ???

				if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
				{
					CVulkanMaterial* pSharedVulkanMat = static_cast<CVulkanMaterial*>(SharedBufferParam.SharedBufferMaterial.get());

					bufferInfoList[LayoutIndex].buffer = pSharedVulkanMat->GetVKUniformBufferList()[FrameIndex][SharedBufferParam.BufferIndex]; // UBOの指定
				}
				else // 通常のバッファ使用
				{
					bufferInfoList[LayoutIndex].buffer = m_VKUniformBufferList[FrameIndex][BufferIndex]; // UBOの指定
				}

				bufferInfoList[LayoutIndex].offset = Layout.second.ByteOffset; // バッファオフセット
				bufferInfoList[LayoutIndex].range = Layout.second.ByteSize; // サイズかな？

				if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
				{
					if (IsUseDynamicOffset())
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
					if (IsUseDynamicOffset())
					{
						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC; // どのタイプのコマンドを発行してもらうのか
					}
					else
					{
						descriptorWrites[LayoutIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // どのタイプのコマンドを発行してもらうのか
					}
				}

				descriptorWrites[LayoutIndex].descriptorCount = 1;
				descriptorWrites[LayoutIndex].pBufferInfo = &bufferInfoList[LayoutIndex];

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

				LayoutIndex++;
			}

			// たぶんバッファの転送を行うコマンドを発行している
			vkUpdateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		return true;
	}

	bool CVulkanMaterial::UpdateTextureBufferDescriptorSets(size_t FrameIndex)
	{
		{
			size_t TexLayoutSize = m_TextureBindingLayoutList.size() * 2; // ImageViewとSamplerがあるので2倍にしている
			std::vector<VkWriteDescriptorSet> descriptorWrites(TexLayoutSize);
			std::vector<VkDescriptorImageInfo> imageInfoList(TexLayoutSize);

			for (int ImageInfoIndex = 0, TextureBindingLayoutIndex = 0; ImageInfoIndex < TexLayoutSize; ImageInfoIndex += 2, TextureBindingLayoutIndex++)
			{
				const auto& TexLayout = m_TextureBindingLayoutList[TextureBindingLayoutIndex];

				std::shared_ptr<graphics::CTexture> Texture = nullptr;
				int TextureIndex = TexLayout.TextureIndex;

				if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
				{
					const auto& it = m_RefTextureMap.find(TexLayout.TextureName);

					Texture = (it != m_RefTextureMap.end()) ? it->second : m_EmptyTexture;
				}
				else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
				{
					const auto& it = m_RefCubeMapMap.find(TexLayout.TextureName);

					Texture = (it != m_RefCubeMapMap.end()) ? it->second : m_EmptyCubeTexture;
				}
				else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
				{
					const auto& it = m_RefFrameTextureMap.find(TexLayout.TextureName);

					Texture = (it != m_RefFrameTextureMap.end()) ? it->second : m_EmptyTexture;
				}
				else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse)
				{
					Texture = (m_RefDiffuse_Tex) ? m_RefDiffuse_Tex : m_EmptyTexture;
				}
				else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular)
				{
					Texture = (m_RefSpecular_Tex) ? m_RefSpecular_Tex : m_EmptyTexture;
				}
				else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT)
				{
					Texture = (m_RefGGXLUT_Tex) ? m_RefGGXLUT_Tex : m_EmptyTexture;
				}

				if (!Texture)
				{
					Console::Log("[ERROR] Texture is nullptr\n");
					return false;
				}

				api::CVulkanTexture* pVulkanTexture = static_cast<api::CVulkanTexture*>(Texture.get());

				{
					descriptorWrites[ImageInfoIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[ImageInfoIndex].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrites[ImageInfoIndex].dstBinding = TexLayout.ViewBindingIndex; // layout(location = n)
					descriptorWrites[ImageInfoIndex].dstArrayElement = 0; // ???

					imageInfoList[ImageInfoIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfoList[ImageInfoIndex].imageView = pVulkanTexture->GetTextureImageView();

					descriptorWrites[ImageInfoIndex].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
					descriptorWrites[ImageInfoIndex].descriptorCount = 1;
					descriptorWrites[ImageInfoIndex].pImageInfo = &imageInfoList[ImageInfoIndex];
				}

				{
					descriptorWrites[ImageInfoIndex + 1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[ImageInfoIndex + 1].dstSet = m_DescriptorSets[FrameIndex]; // どのDescriptorSets(キューファミリが入ってる？)でCPUからGPUにバッファを渡すコマンドを発行するか
					descriptorWrites[ImageInfoIndex + 1].dstBinding = TexLayout.SamplerBindingIndex; // layout(location = n)
					descriptorWrites[ImageInfoIndex + 1].dstArrayElement = 0; // ???

					imageInfoList[ImageInfoIndex + 1].sampler = pVulkanTexture->GetTextureSampler();

					descriptorWrites[ImageInfoIndex + 1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
					descriptorWrites[ImageInfoIndex + 1].descriptorCount = 1;
					descriptorWrites[ImageInfoIndex + 1].pImageInfo = &imageInfoList[ImageInfoIndex + 1];
				}
			}

			// たぶんバッファの転送を行うコマンドを発行している
			vkUpdateDescriptorSets(m_pGraphicsAPI->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}

		return true;
	}

	bool CVulkanMaterial::CreatePipelineLayout()
	{
		// パイプラインレイアウト(Uniformをシェーダーに渡すための仕組み)
		// Uniformの値自体はいつでも変更できるが、どのUniformを使用するかはここで事前にこのパイプラインレイアウトで設定しておく必要がある。
		// たぶんここではLayoutは意味としてUniformを指すのでは？
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		if (IsUseShaderBuffer())
		{
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &GetDescriptorSetLayout();
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
		}
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		VK_CHECK_RESULT(vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));

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

		VK_CHECK_RESULT(vkCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &createInfo, nullptr, &shaderModule));

		return true;
	}
}
#endif