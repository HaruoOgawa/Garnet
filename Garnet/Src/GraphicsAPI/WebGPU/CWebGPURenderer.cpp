#ifdef USE_WEBGPU
#include "CWebGPURenderer.h"
#include "CWebGPUAPI.h"
#include "CWebGPUMaterial.h"
#include "CWebGPUVertexBuffer.h"
#include "CWebGPUIndexBuffer.h"
#include "../../Message/Console.h"

namespace api
{
	CWebGPURenderer::CWebGPURenderer(api::CWebGPUAPI* pGraphicsAPI, const std::string& PassName):
		m_pGraphicsAPI(pGraphicsAPI),
		m_PassName(PassName),
		m_InstanceCount(1),
		m_GraphicsPipeline(nullptr)
	{
	}

	CWebGPURenderer::~CWebGPURenderer()
	{
	}

	bool CWebGPURenderer::Create(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material)
	{
		api::CWebGPUMaterial* pWebGPUMat = static_cast<api::CWebGPUMaterial*>(Material.get());

		m_InstanceCount = VertexBuffer->GetInstanceCount();

		if (!CreateGraphicsPipeline(VertexBuffer, IndexBuffer, pWebGPUMat)) return false; // グラフィックスパイプラインを生成
		
		return true;
	}

	bool CWebGPURenderer::Draw(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum)
	{
		const CWebGPUVertexBuffer* pWebGPUVertexBuffer = static_cast<const CWebGPUVertexBuffer*>(VertexBuffer.get());
		const CWebGPUIndexBuffer* pWebGPUIndexBuffer = static_cast<const CWebGPUIndexBuffer*>(IndexBuffer.get());
		api::CWebGPUMaterial* pWebGPUMat = static_cast<api::CWebGPUMaterial*>(Material.get());

		// ユニフォームバッファの準備
		if (!pWebGPUMat->BuildDrawBuffer(DynamicOffsetNum)) return false;

		// レンダーパスにパイプラインを割り当てる
		wgpuRenderPassEncoderSetPipeline(m_pGraphicsAPI->GetCurrentRenderPass(), m_GraphicsPipeline); 

		// 頂点バッファを割り当てる
		for (int i = 0; i < static_cast<int>(pWebGPUVertexBuffer->GetVertexBufferList().size()); i++)
		{
			wgpuRenderPassEncoderSetVertexBuffer(m_pGraphicsAPI->GetCurrentRenderPass(), i, pWebGPUVertexBuffer->GetVertexBufferList()[i], 0, pWebGPUVertexBuffer->GetVertexBufferSizeList()[i] * sizeof(float));
		}
		
		// インデックスバッファを割り当てる
		if (pWebGPUIndexBuffer->GetIndiceType() == graphics::EIndiceType::UNSIGNED_SHORT)
		{
			wgpuRenderPassEncoderSetIndexBuffer(m_pGraphicsAPI->GetCurrentRenderPass(), pWebGPUIndexBuffer->GetIndexBuffer(), WGPUIndexFormat_Uint16, 0, pWebGPUIndexBuffer->GetIndicesCount() * sizeof(uint16_t));
		}
		else if (pWebGPUIndexBuffer->GetIndiceType() == graphics::EIndiceType::UNSIGNED_INT)
		{
			wgpuRenderPassEncoderSetIndexBuffer(m_pGraphicsAPI->GetCurrentRenderPass(), pWebGPUIndexBuffer->GetIndexBuffer(), WGPUIndexFormat_Uint32, 0, pWebGPUIndexBuffer->GetIndicesCount() * sizeof(uint32_t));
		}

		// バインドグループを割り当てる
		if (pWebGPUMat->IsUseShaderBuffer())
		{
			std::vector<uint32_t> dynamicOffsetList;
			for (const auto& Size : pWebGPUMat->GetBindingRefSizeList())
			{
				uint32_t dynamicOffset = (DynamicOffsetNum - 1) * Size;
				dynamicOffsetList.push_back(dynamicOffset);
			}

			if (pWebGPUMat->IsUseDynamicOffset())
			{
				wgpuRenderPassEncoderSetBindGroup(m_pGraphicsAPI->GetCurrentRenderPass(), 0, pWebGPUMat->GetBindGroup(), static_cast<uint32_t>(dynamicOffsetList.size()), &dynamicOffsetList[0]);
			}
			else
			{
				wgpuRenderPassEncoderSetBindGroup(m_pGraphicsAPI->GetCurrentRenderPass(), 0, pWebGPUMat->GetBindGroup(), 0, nullptr);
			}
		}

		// 描画を実行
		wgpuRenderPassEncoderDrawIndexed(m_pGraphicsAPI->GetCurrentRenderPass(), static_cast<uint32_t>(pWebGPUIndexBuffer->GetIndicesCount()), m_InstanceCount, 0, 0, 0);

		return true;
	}

	bool CWebGPURenderer::UpdateVertexBuffer(const std::vector<float>& PosAttribute, const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer)
	{
		const CWebGPUVertexBuffer* pWebGPUVertexBuffer = static_cast<const CWebGPUVertexBuffer*>(VertexBuffer.get());

		const auto& BufferList = pWebGPUVertexBuffer->GetVertexBufferList();

		// ひとまず0番目に頂点位置が入っている前提でコピーを行う
		// 後ほど頂点バッファの列挙型を導入する
		if (BufferList.size() > 0)
		{
			wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), BufferList[0], 0, &PosAttribute[0], sizeof(float) * PosAttribute.size());
		}

		return true;
	}

	// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
	bool CWebGPURenderer::CreateGraphicsPipeline(const std::shared_ptr<graphics::CVertexBuffer>& VertexBuffer, const std::shared_ptr<graphics::CIndexBuffer>& IndexBuffer, api::CWebGPUMaterial* pWebGPUMat)
	{
		const CWebGPUVertexBuffer* pWebGPUVertexBuffer = static_cast<const CWebGPUVertexBuffer*>(VertexBuffer.get());
		const CWebGPUIndexBuffer* pWebGPUIndexBuffer = static_cast<const CWebGPUIndexBuffer*>(IndexBuffer.get());

		// パイプラインの設定 //////////////////////////////////////////////////////////////////////////
		WGPURenderPipelineDescriptor pipelineDesc{};
		pipelineDesc.nextInChain = nullptr; // 拡張機
		
		// 頂点バッファレイアウト
		std::vector<WGPUVertexBufferLayout> vertexBufferLayouts(pWebGPUVertexBuffer->GetVertexBufferList().size());
		std::vector<WGPUVertexAttribute> attributes(pWebGPUVertexBuffer->GetVertexBufferList().size()); // ここベクターにしないとなんかvertexBufferLayoutsに入れておいてもメモリが解放されててなんか数値がおかしなことに・・・
		// ↑↑↑ 確かにスタックメモリに格納する変数はスコープを抜けたら解放されるよね・・・
		// そしてその解放されたものを使用していると当然おかしくなる
		// メモリの解放タイミングと使用タイミングには留意しよう！

		for (int i = 0; i < static_cast<int>(pWebGPUVertexBuffer->GetVertexBufferList().size()); i++)
		{
			//
			int Dimension = pWebGPUVertexBuffer->GetAttributeDimensions()[i];
			int ByteStride = pWebGPUVertexBuffer->GetAttribByteStrides()[i];

			//
			attributes[i].shaderLocation = i; // Shaderでのアトリビュートインデックス
			attributes[i].format = GetVertexFormat(Dimension, pWebGPUVertexBuffer->GetAttribDataTypes()[i]);
			attributes[i].offset = 0;

			//
			vertexBufferLayouts[i].attributeCount = 1;
			vertexBufferLayouts[i].attributes = &attributes[i];
			vertexBufferLayouts[i].arrayStride = (ByteStride != 0) ? ByteStride : (Dimension * sizeof(pWebGPUVertexBuffer->GetVertices()[i][0])); // ストライドとは連続する要素間のバイト数のこと
			vertexBufferLayouts[i].stepMode = WGPUVertexStepMode_Vertex; // ??? 頂点データが同じインスタンスなら共有されることを示す設定 ???
		}

		// 頂点シェーダー
		pipelineDesc.vertex.nextInChain = nullptr;
		pipelineDesc.vertex.bufferCount = static_cast<uint32_t>(vertexBufferLayouts.size()); // 頂点バッファ
		pipelineDesc.vertex.buffers = &vertexBufferLayouts[0];
		pipelineDesc.vertex.module = pWebGPUMat->GetVertexShaderModele(); // 頂点シェーダー
		pipelineDesc.vertex.entryPoint = "main";
		pipelineDesc.vertex.constantCount = 0; // ??? ユニフォームの指定に使用するやつかな？
		pipelineDesc.vertex.constants = nullptr;

		// プリミティブの設定
		pipelineDesc.primitive.nextInChain = nullptr;
		pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList; // トポロジー
		pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined; // インデックスバッファの型かな
		pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW; // カリングの方向
		
		switch (pWebGPUMat->GetCullMode())
		{
		case graphics::ECullMode::CULL_BACK:
			pipelineDesc.primitive.cullMode = WGPUCullMode_Back; // カリングモードの設定
			break;

		case graphics::ECullMode::CULL_FRONT:
			pipelineDesc.primitive.cullMode = WGPUCullMode_Front; // カリングモードの設定
			break;

		case graphics::ECullMode::CULL_NONE:
			pipelineDesc.primitive.cullMode = WGPUCullMode_None; // カリングモードの設定
			break;

		default:
			pipelineDesc.primitive.cullMode = WGPUCullMode_Back; // カリングモードの設定
			break;
		}

		// ステンシルバッファ・デプスバッファ
		WGPUDepthStencilState depthStencilState;
		SetDefaultDepthStencil(depthStencilState);
		depthStencilState.nextInChain = nullptr;
		depthStencilState.depthCompare = (pWebGPUMat->IsEnabledZTest())? WGPUCompareFunction_Less : WGPUCompareFunction_Always;
		//depthStencilState.depthCompare = WGPUCompareFunction_Less;
		//depthStencilState.depthWriteEnabled = pWebGPUMat->IsEnabledZTest();
		depthStencilState.depthWriteEnabled = true;
		WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;
		depthStencilState.format = depthTextureFormat;
		depthStencilState.stencilReadMask = 0; // ステンシルバッファの読み書きをオフにしておく
		depthStencilState.stencilWriteMask = 0;

		pipelineDesc.depthStencil = &depthStencilState;

		// ブレンディング
		// <計算式> rgba = srcFactor * rgba [operation] dstFactor * rgba
		WGPUBlendState blendState{};
		switch (pWebGPUMat->GetBlendType())
		{
			case graphics::EBlendType::BLEND_TYPE_ADDITIVE:
				blendState.color.srcFactor = WGPUBlendFactor_One;
				blendState.color.dstFactor = WGPUBlendFactor_Zero;
				blendState.color.operation = WGPUBlendOperation_Add;

				blendState.alpha.srcFactor = WGPUBlendFactor_One;
				blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
				blendState.alpha.operation = WGPUBlendOperation_Add;

				break;
			case graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA:
				blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
				blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
				blendState.color.operation = WGPUBlendOperation_Add;

				blendState.alpha.srcFactor = WGPUBlendFactor_One;
				blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
				blendState.alpha.operation = WGPUBlendOperation_Add;

				break;
			default:
				blendState.color.srcFactor = WGPUBlendFactor_One;
				blendState.color.dstFactor = WGPUBlendFactor_Zero;
				blendState.color.operation = WGPUBlendOperation_Add;

				blendState.alpha.srcFactor = WGPUBlendFactor_One;
				blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
				blendState.alpha.operation = WGPUBlendOperation_Add;

				break;
		}
		

		WGPUColorTargetState colorTarget{};
		colorTarget.nextInChain = nullptr;
		colorTarget.format = m_pGraphicsAPI->GetSwapChainFormat();
		colorTarget.blend = &blendState;
		colorTarget.writeMask = WGPUColorWriteMask_All;

		// マルチサンプリング(MSAA)
		pipelineDesc.multisample.nextInChain = nullptr;
		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // ??? Bit Mask ???
		pipelineDesc.multisample.alphaToCoverageEnabled = false; // ???

		// フラグメントシェーダー
		WGPUFragmentState fragmentState{};
		fragmentState.nextInChain = nullptr;
		fragmentState.module = pWebGPUMat->GetFragmentShaderModele();
		fragmentState.entryPoint = "main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;

		pipelineDesc.fragment = &fragmentState;
		
		// パイプラインレイアウトの指定
		// パイプラインレイアウトは、レンダリングパイプラインで使用されるすべてのリソースをどのようにバインドする必要があるかを示す
		WGPUPipelineLayoutDescriptor layoutDesc{};
		layoutDesc.nextInChain = nullptr;
		if (pWebGPUMat->IsUseShaderBuffer())
		{
			layoutDesc.bindGroupLayoutCount = 1;
			layoutDesc.bindGroupLayouts = &pWebGPUMat->GetBindGroupLayout();//&m_BindGroupLayout;
		}
		else
		{
			layoutDesc.bindGroupLayoutCount = 0;
			layoutDesc.bindGroupLayouts = nullptr;
		}
		
		WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &layoutDesc);

		pipelineDesc.layout = layout;

		// パイプラインの生成 /////////////////////////////////////////////////////////////////////////////
		m_GraphicsPipeline = wgpuDeviceCreateRenderPipeline(m_pGraphicsAPI->GetLogicalDevice(), &pipelineDesc);

		if (!m_GraphicsPipeline)
		{
			Console::Log("[Error] m_GraphicsPipeline is null\n");
			return false;
		}

		return true;
	}

	// Helper Function ///////////////////////////////////////////////////////////////////////
	WGPUVertexFormat CWebGPURenderer::GetVertexFormat(int Dimention, graphics::EDataType DataType)
	{
		WGPUVertexFormat result;

		switch (DataType)
		{
		case graphics::EDataType::TYPE_SIGNED_BYTE:
			if (Dimention == 1)
			{
				//result = WGPUVertexFormat_Sint8;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Sint8x2;
			}
			else if (Dimention == 3)
			{
				//result = WGPUVertexFormat_Sint8x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Sint8x4;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_BYTE:
			if (Dimention == 1)
			{
				//result = WGPUVertexFormat_Uint8;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Uint8x2;
			}
			else if (Dimention == 3)
			{
				//result = WGPUVertexFormat_Uint8x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Uint8x4;
			}
			break;
		case graphics::EDataType::TYPE_SIGNED_SHORT:
			if (Dimention == 1)
			{
				//result = WGPUVertexFormat_Sint16;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Sint16x2;
			}
			else if (Dimention == 3)
			{
				//result = WGPUVertexFormat_Sint16x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Sint16x4;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_SHORT:
			if (Dimention == 1)
			{
				//result = WGPUVertexFormat_Uint16;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Uint16x2;
			}
			else if (Dimention == 3)
			{
				//result = WGPUVertexFormat_Uint16x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Uint16x4;
			}
			break;
		case graphics::EDataType::TYPE_UNSIGNED_INT:
			if (Dimention == 1)
			{
				result = WGPUVertexFormat_Uint32;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Uint32x2;
			}
			else if (Dimention == 3)
			{
				result = WGPUVertexFormat_Uint32x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Uint32x4;
			}
			break;
		case graphics::EDataType::TYPE_FLOAT:
		default:
			if (Dimention == 1)
			{
				result = WGPUVertexFormat_Float32;
			}
			else if (Dimention == 2)
			{
				result = WGPUVertexFormat_Float32x2;
			}
			else if (Dimention == 3)
			{
				result = WGPUVertexFormat_Float32x3;
			}
			else if (Dimention == 4)
			{
				result = WGPUVertexFormat_Float32x4;
			}
			break;
		}

		return result;
	}

	void CWebGPURenderer::SetDefaultDepthStencil(WGPUDepthStencilState& depthStencilState)
	{
		depthStencilState.nextInChain = nullptr;

		depthStencilState.format = WGPUTextureFormat::WGPUTextureFormat_Undefined;
		depthStencilState.depthWriteEnabled = false;
		depthStencilState.depthCompare = WGPUCompareFunction_Always;
		depthStencilState.stencilReadMask = 0xFFFFFFFF;
		depthStencilState.stencilWriteMask = 0xFFFFFFFF;
		depthStencilState.depthBias = 0;
		depthStencilState.depthBiasSlopeScale = 0;
		depthStencilState.depthBiasClamp = 0;

		depthStencilState.stencilFront.compare = WGPUCompareFunction_Always;
		depthStencilState.stencilFront.failOp = WGPUStencilOperation_Keep;
		depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation_Keep;
		depthStencilState.stencilFront.passOp = WGPUStencilOperation_Keep;

		depthStencilState.stencilBack.compare = WGPUCompareFunction_Always;
		depthStencilState.stencilBack.failOp = WGPUStencilOperation_Keep;
		depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation_Keep;
		depthStencilState.stencilBack.passOp = WGPUStencilOperation_Keep;
	}
}
#endif