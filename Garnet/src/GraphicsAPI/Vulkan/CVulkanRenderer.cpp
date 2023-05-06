#ifndef __DAWN__
#include "CVulkanRenderer.h"
#include "CVulkanAPI.h"
#include "CVulkanMaterial.h"
#include "../CRendererCreateInfo.h"

namespace renderer
{
	CVulkanRenderer::CVulkanRenderer():
		m_pGraphicsAPI(nullptr)
	{
	}

	CVulkanRenderer::~CVulkanRenderer()
	{
		Release();
	}

	void CVulkanRenderer::Release()
	{
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

	bool CVulkanRenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		m_pGraphicsAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(Material.get());

		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを作成
		if (!CreateIndexBuffer(createInfo)) return false; // インデックスバッファを作成

		if (!CreateGraphicsPipeline(createInfo, pVulkanMat)) return false; // グラフィックパイプラインを作成

		return true;
	}

	bool CVulkanRenderer::Update(float SecondsTime)
	{
		return true;
	}

	bool CVulkanRenderer::Draw(const std::shared_ptr<graphics::CMaterial>& Material)
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(Material.get());

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
			m_PipelineLayout, 0, 1, &pVulkanMat->GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], 0, nullptr);

		// 描画コマンドを発行
		//vkCmdDraw(m_CommandBuffers[m_CurrentFrame], 3, 1, 0, 0); // パラメーター: vertexCount, instanceCount, firstVertex, firstInstance
		// インデックス付のドローコマンドはこちら
		vkCmdDrawIndexed(m_pGraphicsAPI->GetCommandBuffers()[m_pGraphicsAPI->GetCurrentFrame()], m_IndicesCount, 1, 0, 0, 0);

		return true;
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	bool CVulkanRenderer::CreateGraphicsPipeline(const CRendererCreateInfo& createInfo, api::CVulkanMaterial* pVulkanMat)
	{
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
		pipelineLayoutInfo.pSetLayouts = pVulkanMat->GetDescriptorSetLayout();
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
		pipelineInfo.stageCount = static_cast<uint32_t>(pVulkanMat->GetShaderStages().size()); // しぇだーステージの数
		pipelineInfo.pStages = pVulkanMat->GetShaderStages().data();

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

		return true;
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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