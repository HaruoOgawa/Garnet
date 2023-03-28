#ifdef __DAWN__
#include "CWebGPURenderer.h"
#include "CRendererCreateInfo.h"
#include "../GraphicsAPI/CWebGPUAPI.h"

namespace renderer
{
	CWebGPURenderer::CWebGPURenderer()
	{
	}

	CWebGPURenderer::~CWebGPURenderer()
	{
	}

	bool CWebGPURenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		if (!CreateGraphicsPipeline(createInfo)) return false; // グラフィックスパイプラインを生成

		return true;
	}

	bool CWebGPURenderer::Update()
	{
		return true;
	}

	bool CWebGPURenderer::Draw()
	{
		wgpuRenderPassEncoderSetPipeline(m_pGraphicsAPI->GetRenderPass(), m_GraphicsPipeline);
		wgpuRenderPassEncoderDraw(m_pGraphicsAPI->GetRenderPass(), 3, 1, 0, 0);

		return true;
	}

	// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
	bool CWebGPURenderer::CreateGraphicsPipeline(const CRendererCreateInfo& createInfo)
	{
		// シェーダーモジュールの生成 //////////////////////////////////////////////////////////////////
		WGPUShaderModule vertexShaderModele = CreateShaderModule(createInfo.GetVertexShaderCode());
		WGPUShaderModule fragmentShaderModele = CreateShaderModule(createInfo.GetFragmentShaderCode());

		// パイプラインの設定 //////////////////////////////////////////////////////////////////////////
		WGPURenderPipelineDescriptor pipelineDesc{};
		pipelineDesc.nextInChain = nullptr; // 拡張機能

		// 頂点バッファ, 頂点シェーダー
		pipelineDesc.vertex.bufferCount = 0; // 頂点バッファ
		pipelineDesc.vertex.buffers = nullptr;
		pipelineDesc.vertex.module = vertexShaderModele; // 頂点シェーダー
		pipelineDesc.vertex.entryPoint = "main";
		pipelineDesc.vertex.constantCount = 0; // ??? ユニフォームの指定に使用するやつかな？
		pipelineDesc.vertex.constants = nullptr;

		// プリミティブの設定
		pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList; // トポロジー
		pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined; // インデックスバッファの型かな
		pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW; // カリングの方向
		pipelineDesc.primitive.cullMode = WGPUCullMode_None; // カリングモードの設定

		// ステンシルバッファ・デプスバッファ
		pipelineDesc.depthStencil = nullptr;

		// ブレンディング
		// <計算式> rgba = srcFactor * rgba [operation] dstFactor * rgba
		WGPUBlendState blendState{};
		blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
		blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrc;
		blendState.color.operation = WGPUBlendOperation_Add;

		blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
		blendState.alpha.dstFactor = WGPUBlendFactor_One;
		blendState.alpha.operation = WGPUBlendOperation_Add;

		WGPUColorTargetState colorTarget{};
		colorTarget.format = m_pGraphicsAPI->GetSwapChainFormat();
		colorTarget.blend = &blendState;
		colorTarget.writeMask = WGPUColorWriteMask_All;

		// マルチサンプリング(MSAA)
		pipelineDesc.multisample.count = 1;
		pipelineDesc.multisample.mask = ~0u; // ??? Bit Mask ???
		pipelineDesc.multisample.alphaToCoverageEnabled = false; // ???

		// フラグメントシェーダー
		WGPUFragmentState fragmentState{};
		fragmentState.module = fragmentShaderModele;
		fragmentState.entryPoint = "main";
		fragmentState.constantCount = 0;
		fragmentState.constants = nullptr;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTarget;

		pipelineDesc.fragment = &fragmentState;
		
		// パイプラインレイアウトの指定(バッファへのメモリアクセスを制御する機構)
		WGPUPipelineLayoutDescriptor layoutDesc{};
		layoutDesc.nextInChain = nullptr;
		layoutDesc.bindGroupLayoutCount = 0;
		layoutDesc.bindGroupLayouts = nullptr;
		WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &layoutDesc);

		pipelineDesc.layout = layout;

		// パイプラインの生成 /////////////////////////////////////////////////////////////////////////////
		m_GraphicsPipeline = wgpuDeviceCreateRenderPipeline(m_pGraphicsAPI->GetLogicalDevice(), &pipelineDesc);

		return true;
	}

	// Helper Function ///////////////////////////////////////////////////////////////////////
	WGPUShaderModule CWebGPURenderer::CreateShaderModule(const std::string& shaderCode)
	{
		WGPUShaderModuleWGSLDescriptor shaderCodeDesc{};
		shaderCodeDesc.chain.next = nullptr; // ???
		shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
		shaderCodeDesc.code = &shaderCode[0]; // Shaderコードを渡す

		WGPUShaderModuleDescriptor shaderDesc{};
		shaderDesc.hintCount = 0; // ???
		shaderDesc.hints = nullptr;
		shaderDesc.nextInChain = &shaderCodeDesc.chain; // 使用するShaderの種類を指定(ここででWGSL)。WGPUShaderModuleSPIRVDescriptorを指定することでVulkanのSplivが使用できる(SplivはGLSLからコンパイルできて便利)

		WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &shaderDesc);

		return shaderModule;
	}
}
#endif