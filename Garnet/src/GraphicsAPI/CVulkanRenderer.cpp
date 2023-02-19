#include "CVulkanRenderer.h"
#include "../GraphicsAPI/CVulkanAPI.h"
#include "CRendererCreateInfo.h"


namespace renderer
{
	CVulkanRenderer::CVulkanRenderer()
	{
	}

	CVulkanRenderer::~CVulkanRenderer()
	{
	}

	bool CVulkanRenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		auto api = static_cast<api::CVulkanAPI*>(pGraphicsAPI);

		if (!CreateDescriptorSetLayout(api, createInfo)) return false; // UBO(Uniform Buffer Object)をどのようにバインドするか
		if (!CreateGraphicsPipeline(api, createInfo)) return false; // グラフィックパイプラインを作成
		return true;
	}

	// 初期化関数 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanRenderer::CreateDescriptorSetLayout(api::CVulkanAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		//
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		// レイアウトのバインドに関する設定
		// Model, View Proj等のUBOのレイアウト
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0; // バインディングインデックス？
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // 画像のサンプリングに使用するフィールド

		bindings.push_back(uboLayoutBinding);

		// TextureSampler用のレイアウトを設定
		if (createInfo.IsUseMainTexture())
		{
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = 1; //バインディングインデックス. 上のやつが0だから1を設定
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			bindings.push_back(samplerLayoutBinding);
		}

		// レイアウトの作成に関する設定
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(pGraphicsAPI->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout);

		return (result == VK_SUCCESS);
	}

	bool CVulkanRenderer::CreateGraphicsPipeline(api::CVulkanAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		// シェーダーの準備
		auto VertexShadeCode = createInfo.GetVertexShaderCode();
		auto FragShadeCode = createInfo.GetFragmentShaderCode();

		// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
		// 使う時にGeometryとかTessellationも追加する
		VkShaderModule vertShaderModule;
		const bool UseVertexShader = CreateShaderModule(vertShaderModule, pGraphicsAPI, VertexShadeCode);
		
		VkShaderModule fragShaderModule;
		const bool UseFragmentShader = CreateShaderModule(fragShaderModule, pGraphicsAPI, FragShadeCode);

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

		// グラフィックパイプラインの固定機能の設定 ///////////////////////////////////////////////////////////////////////////////////////

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
		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDestriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInto{};
		vertexInputInto.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInto.vertexBindingDescriptionCount = 1; // 頂点バッファのバインドに関するの設定
		vertexInputInto.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInto.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());  // 頂点データ(アトリビュート)の設定
		vertexInputInto.pVertexAttributeDescriptions = attributeDescriptions.data();

		// 入力アセンブリ(頂点から描画されるジオメトリの種類など, GL_TRIANGLE_STRIPみたいなのを設定する場所)
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE; // Trueにすると自分で定義したインデックスバッファが使用できる？

		// ビューポートの設定
		// 上記のダイナミックステートのことで動的変更を可能にする
		VkViewport viewport{};
		viewport.x = 0.0f; // 基準の座標
		viewport.y = 0.0f;
		viewport.width = (float)pGraphicsAPI->GetSwapChainExtent().width;
		viewport.height = (float)pGraphicsAPI->GetSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// シザーの設定(シザーとはピクセルが実際に格納される領域を定義する. シザーよりも外側の領域はラスタライザにより破棄される)
		// 上記のダイナミックステートのことで動的変更を可能にする
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = pGraphicsAPI->GetSwapChainExtent(); // 解像度

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

		if (vkCreatePipelineLayout(pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) return false;

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

		pipelineInfo.renderPass = pGraphicsAPI->GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // パイプラインから新しいパイプラインを派生して作成するためのフィールド?
		pipelineInfo.basePipelineIndex = -1; // 今は何もしていない

		if (vkCreateGraphicsPipelines(pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) return false;

		// シェーダーモジュールを破棄する
		vkDestroyShaderModule(pGraphicsAPI->GetLogicalDevice(), fragShaderModule, nullptr);
		vkDestroyShaderModule(pGraphicsAPI->GetLogicalDevice(), vertShaderModule, nullptr);

		return true;
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader
	// ShaderModuleの作成(Shaderをラップ・管理するためのもの)
	bool CVulkanRenderer::CreateShaderModule(VkShaderModule& shaderModule, api::CVulkanAPI* pGraphicsAPI, const std::vector<unsigned char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(pGraphicsAPI->GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanRenderer::Draw()
	{
		return true;
	}
}