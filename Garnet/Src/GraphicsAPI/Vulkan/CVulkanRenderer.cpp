#ifdef USE_VULKAN
#include "CVulkanRenderer.h"
#include "CVulkanAPI.h"
#include "CVulkanRenderPass.h"
#include "CVulkanMaterial.h"
#include "CVulkanVertexBuffer.h"
#include "CVulkanIndexBuffer.h"

namespace api
{
	CVulkanRenderer::CVulkanRenderer(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName):
		m_pGraphicsAPI(pGraphicsAPI),
		m_PassName(PassName),
		m_DynamicOffsetNum(0),
		m_InstanceCount(1),
		m_PipelineLayout(nullptr),
		m_GraphicsPipeline(nullptr)
	{
	}

	CVulkanRenderer::~CVulkanRenderer()
	{
		Release();
	}

	void CVulkanRenderer::Release()
	{
		// グラフィックパイプラインの破棄
		if (m_GraphicsPipeline)
		{
			vkDestroyPipeline(m_pGraphicsAPI->GetLogicalDevice(), m_GraphicsPipeline, nullptr);
			m_GraphicsPipeline = nullptr;
		}

		// パイプラインレイアウトの破棄(たぶん本来は3Dオブジェクトごとにあるやつ) 
		if (m_PipelineLayout)
		{
			vkDestroyPipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), m_PipelineLayout, nullptr);
			m_PipelineLayout = nullptr;
		}
	}

	bool CVulkanRenderer::Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(Material.get());

		m_InstanceCount = VertexBuffer->GetInstanceCount();

		if (!CreateGraphicsPipeline(VertexBuffer, IndexBuffer, pVulkanMat)) return false; // グラフィックパイプラインを作成

		return true;
	}

	bool CVulkanRenderer::Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum)
	{
		const CVulkanVertexBuffer* pVulkanVertexBuffer = static_cast<const CVulkanVertexBuffer*>(VertexBuffer.get());
		const CVulkanIndexBuffer* pVulkanIndexBuffer = static_cast<const CVulkanIndexBuffer*>(IndexBuffer.get());
		api::CVulkanMaterial* pVulkanMat = static_cast<api::CVulkanMaterial*>(Material.get());

		// ユニフォームバッファの準備
		if (!pVulkanMat->BuildDrawBuffer(DynamicOffsetNum)) return false;

		// グラフィックパイプラインをコマンドにバインド
		vkCmdBindPipeline(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// カリングモードを設定
		SetCullMode(pVulkanMat->GetCullMode());
		
		// 頂点バッファをパイプラインにバインドする
		VkDeviceSize offsets[] = { 0 };
		for (int i = 0; i < static_cast<int>(pVulkanVertexBuffer->GetVertexBufferList().size()); i++)
		{
			vkCmdBindVertexBuffers(m_pGraphicsAPI->GetCurrentCommandBuffer(), i, 1, &pVulkanVertexBuffer->GetVertexBufferList()[i], offsets);
		}

		// インデックスバッファをパイプラインにバインドする
		if (pVulkanIndexBuffer->GetIndiceType() == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			vkCmdBindIndexBuffer(m_pGraphicsAPI->GetCurrentCommandBuffer(), pVulkanIndexBuffer->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
		}
		else if (pVulkanIndexBuffer->GetIndiceType() == graphics::EIndiceType::UNSIGNED_INT)
		{
			vkCmdBindIndexBuffer(m_pGraphicsAPI->GetCurrentCommandBuffer(), pVulkanIndexBuffer->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
		
		// UBOのセット
		if (pVulkanMat->IsUseShaderBuffer())
		{
			std::vector<uint32_t> dynamicOffsetList;
			for (const auto& Size : pVulkanMat->GetBindingRefSizeList())
			{
				uint32_t dynamicOffset = (DynamicOffsetNum - 1) * Size;
				dynamicOffsetList.push_back(dynamicOffset);
			}

			if (pVulkanMat->IsUseDynamicOffset())
			{
				vkCmdBindDescriptorSets(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_PipelineLayout, 0, 1, &pVulkanMat->GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], static_cast<uint32_t>(dynamicOffsetList.size()), &dynamicOffsetList[0]);
			}
			else
			{
				vkCmdBindDescriptorSets(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_PipelineLayout, 0, 1, &pVulkanMat->GetDescriptorSets()[m_pGraphicsAPI->GetCurrentFrame()], 0, nullptr);
			}
		}

		// 描画コマンドを発行
		//vkCmdDraw(m_CommandBuffers[m_CurrentFrame], 3, 1, 0, 0); // パラメーター: vertexCount, instanceCount, firstVertex, firstInstance
		// インデックス付のドローコマンドはこちら
		vkCmdDrawIndexed(m_pGraphicsAPI->GetCurrentCommandBuffer(), pVulkanIndexBuffer->GetIndicesCount(), m_InstanceCount, 0, 0, 0);

		return true;
	}

	bool CVulkanRenderer::UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer)
	{
		const CVulkanVertexBuffer* pVulkanVertexBuffer = static_cast<const CVulkanVertexBuffer*>(VertexBuffer.get());

		const auto& MemoryList = pVulkanVertexBuffer->GetVertexBufferMemoryList();

		// ひとまず0番目に頂点位置が入っている前提でコピーを行う
		// 後ほど頂点バッファの列挙型を導入する
		if (MemoryList.size() > 0)
		{
			size_t bufferSize = sizeof(float) * PosAttribute.size();

			// 
			VkBuffer stagingBuffer;
			VkDeviceMemory statingBufferMemory;
			m_pGraphicsAPI->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, statingBufferMemory);

			void* data;

			vkMapMemory(m_pGraphicsAPI->GetLogicalDevice(), statingBufferMemory, 0, bufferSize, 0, &data);
			std::memcpy(data, &PosAttribute[0], bufferSize);
			vkUnmapMemory(m_pGraphicsAPI->GetLogicalDevice(), statingBufferMemory);

			//
			m_pGraphicsAPI->CopyBuffer(stagingBuffer, pVulkanVertexBuffer->GetVertexBufferList()[0], bufferSize);
		}

		return true;
	}

	// Vulkanメインロジック /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool CVulkanRenderer::CreateGraphicsPipeline(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, api::CVulkanMaterial* pVulkanMat)
	{
		const CVulkanVertexBuffer* pVulkanVertexBuffer = static_cast<const CVulkanVertexBuffer*>(VertexBuffer.get());
		const CVulkanIndexBuffer* pVulkanIndexBuffer = static_cast<const CVulkanIndexBuffer*>(IndexBuffer.get());

		// グラフィックパイプラインの固定機の設定 ///////////////////////////////////////////////////////////////////////////////////////
		// 動的状態(ダイナミックステート)の設定(パイプラインにベイクせずにマイフレームの描画時に設定できるようにするパラメーターの設定)
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_CULL_MODE
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		// 頂点バッファ入力(Vertex Shaderに渡すデータ形式について設定する)
		int Size = static_cast<int>(pVulkanVertexBuffer->GetVertexBufferList().size());
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(Size);
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(Size);

		for (int i = 0; i < Size; i++)
		{
			//
			int Dimension = pVulkanVertexBuffer->GetAttributeDimensions()[i];
			int ByteStride = pVulkanVertexBuffer->GetAttribByteStrides()[i];

			// 頂点バッファのバインドに関する説明,設定(頂点バッファレイアウト)
			bindingDescriptions[i].binding = i; // バインドする頂点バッファのインデックス(?)違う形式で頂点バッファを用意するときに使用する？
			bindingDescriptions[i].stride = (ByteStride != 0)? ByteStride : (Dimension * sizeof(pVulkanVertexBuffer->GetVertices()[i][0])); // 頂点バッファ内の要素一つあたりのサイズ。次の要素までのバイト数
			bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // よくわからぬ。各頂点の後、次のデータ エントリに移動します。らしい

			// アトリビュート(頂点データ)の設定
			attributeDescriptions[i].binding = i; // BindingDescriptionの内どのバインド設定を使用するかのインデックス
			attributeDescriptions[i].location = i; // Shaderのlayout(location = 0)に設定すｒ数値
			attributeDescriptions[i].format = GetVertexFormat(Dimension, pVulkanVertexBuffer->GetAttribDataTypes()[i]); // データ型. SFLOAT --> Signed Float
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
		
		switch (pVulkanMat->GetCullMode())
		{
			case graphics::ECullMode::CULL_BACK :
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // カリングの設定
				break;

			case graphics::ECullMode::CULL_FRONT :
				rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT; // カリングの設定
				break;

			case graphics::ECullMode::CULL_NONE :
				rasterizer.cullMode = VK_CULL_MODE_NONE; // カリングの設定
				break;

			default:
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // カリングの設定
				break;
		}

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
		colorBlendAttachment.blendEnable = VK_TRUE;

		switch (pVulkanMat->GetBlendType())
		{
		case graphics::EBlendType::BLEND_TYPE_ADDITIVE:
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			break;
		case graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA:
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			break;
		default:
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

			break;
		}
		
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
		if (pVulkanMat->IsUseShaderBuffer())
		{
			pipelineLayoutInfo.setLayoutCount = 1;
			pipelineLayoutInfo.pSetLayouts = &pVulkanMat->GetDescriptorSetLayout();
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0;
			pipelineLayoutInfo.pSetLayouts = nullptr;
		}
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) return false;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// レンダリングパイプラインでデプスとステンシルを有効にする
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencil.depthTestEnable = (pVulkanMat->IsEnabledZTest()) ? VK_TRUE : VK_FALSE;
		depthStencil.depthTestEnable = VK_TRUE;
		//depthStencil.depthWriteEnable = (pVulkanMat->IsEnabledZTest()) ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = (pVulkanMat->IsEnabledZTest()) ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_ALWAYS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		// これまでの情報をもとにレンダリングパイプラインを構築
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(pVulkanMat->GetShaderStages().size()); // シェーダーステージの数
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

		if (!m_PassName.empty())
		{
			const auto& RenderPassMap = m_pGraphicsAPI->GetOffScreenRenderPassMap();
			const auto& RenderPass = RenderPassMap.find(m_PassName);
			if (RenderPass != RenderPassMap.end())
			{
				api::CVulkanRenderPass* pVulkanRenderPass = static_cast<api::CVulkanRenderPass*>(RenderPass->second.get());
				if (pVulkanRenderPass) pipelineInfo.renderPass = pVulkanRenderPass->GetRenderPass();
			}
			else
			{
				// デフォルトレンダーパスを使用(スワップチェーンに渡すやつ)
				pipelineInfo.renderPass = m_pGraphicsAPI->GetSwapChainRenderPass();
			}
		}
		else
		{
			// デフォルトレンダーパスを使用(スワップチェーンに渡すやつ)
			pipelineInfo.renderPass = m_pGraphicsAPI->GetSwapChainRenderPass();
		}
		
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // パイプラインから新しいパイプラインを派生して作成するためのフィールド?
		pipelineInfo.basePipelineIndex = -1; // 今は何もしていない

		if (vkCreateGraphicsPipelines(m_pGraphicsAPI->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) return false;

		return true;
	}

	void CVulkanRenderer::SetCullMode(graphics::ECullMode CullMode)
	{
		switch (CullMode)
		{
		case graphics::ECullMode::CULL_BACK:
			vkCmdSetCullMode(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_CULL_MODE_BACK_BIT);
			break;

		case graphics::ECullMode::CULL_FRONT:
			vkCmdSetCullMode(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_CULL_MODE_FRONT_BIT);
			break;

		case graphics::ECullMode::CULL_NONE:
			vkCmdSetCullMode(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_CULL_MODE_NONE);
			break;

		default:
			vkCmdSetCullMode(m_pGraphicsAPI->GetCurrentCommandBuffer(), VK_CULL_MODE_BACK_BIT);
			break;
		}
	}

	// ヘルパー関数 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VkFormat CVulkanRenderer::GetVertexFormat(int Dimention, graphics::EDataType DataType)
	{
		VkFormat result = VK_FORMAT_UNDEFINED;

		switch (DataType)
		{
		case graphics::EDataType::TYPE_SIGNED_BYTE:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R8_SINT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R8G8_SINT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R8G8B8_SINT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R8G8B8A8_SINT;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_BYTE:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R8_UINT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R8G8_UINT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R8G8B8_UINT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R8G8B8A8_UINT;
			}
			break;
		case graphics::EDataType::TYPE_SIGNED_SHORT:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R16_SINT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R16G16_SINT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R16G16B16_SINT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R16G16B16A16_SINT;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_SHORT:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R16_UINT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R16G16_UINT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R16G16B16_UINT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R16G16B16A16_UINT;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_INT:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R32_UINT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R32G32_UINT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R32G32B32_UINT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R32G32B32A32_UINT;
			}
			break;
		case graphics::EDataType::TYPE_FLOAT:
		default:
			if (Dimention == 1)
			{
				result = VK_FORMAT_R32_SFLOAT;
			}
			else if (Dimention == 2)
			{
				result = VK_FORMAT_R32G32_SFLOAT;
			}
			else if (Dimention == 3)
			{
				result = VK_FORMAT_R32G32B32_SFLOAT;
			}
			else if (Dimention == 4)
			{
				result = VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			break;
		}

		return result;
	}
}

#endif