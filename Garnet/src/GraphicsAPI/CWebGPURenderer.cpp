#ifdef __DAWN__
#include "CWebGPURenderer.h"
#include "CRendererCreateInfo.h"
#include "../GraphicsAPI/CWebGPUAPI.h"
#include "../Debug/Message/Console.h"

namespace renderer
{
	CWebGPURenderer::CWebGPURenderer():
		m_pGraphicsAPI(nullptr),
		m_GraphicsPipeline(nullptr),
		m_VertexCount(0),
		m_IndexBuffer(nullptr),
		m_IndexCount(0)
	{
	}

	CWebGPURenderer::~CWebGPURenderer()
	{
	}

	bool CWebGPURenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを生成
		if (!CreateIndexBuffer(createInfo)) return false; //インデックスバッファを生成
		if (!CreateGraphicsPipeline(createInfo)) return false; // グラフィックスパイプラインを生成
		
		return true;
	}

	bool CWebGPURenderer::Update()
	{
		return true;
	}

	bool CWebGPURenderer::Draw()
	{
		// レンダーパスにパイプラインを割り当てる
		wgpuRenderPassEncoderSetPipeline(m_pGraphicsAPI->GetRenderPass(), m_GraphicsPipeline); 

		// 頂点バッファを割り当てる
		for (int i = 0; i < static_cast<int>(m_VertexBufferList.size()); i++)
		{
			wgpuRenderPassEncoderSetVertexBuffer(m_pGraphicsAPI->GetRenderPass(), i, m_VertexBufferList[i], 0, m_VertexBufferSizeList[i] * sizeof(float));
		}
		
		// インデックスバッファを割り当てる
		wgpuRenderPassEncoderSetIndexBuffer(m_pGraphicsAPI->GetRenderPass(), m_IndexBuffer, WGPUIndexFormat_Uint16, 0, m_IndexCount * sizeof(uint16_t));

		// 描画を実行
		wgpuRenderPassEncoderDrawIndexed(m_pGraphicsAPI->GetRenderPass(), static_cast<uint32_t>(m_IndexCount), 1, 0, 0, 0);

		return true;
	}

	// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
	bool CWebGPURenderer::CreateVertexBuffer(const CRendererCreateInfo& createInfo)
	{
		// 頂点バッファオブジェクトの生成
		for (const auto& Data : createInfo.GetVertices())
		{
			// WGPUBufferUsage_CopyDst はCPUからGPUへメモリをコピーすることを指定する
			// 反対にGPUからCPUへ読み戻したい場合はWGPUBufferUsage_CopySrcも指定する

			WGPUBuffer Buffer;
			if (!CreateBuffer(Buffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex, &Data[0], Data.size() * sizeof(float))) return false;

			// バッファを保存
			m_VertexBufferList.push_back(Buffer);
			m_VertexBufferSizeList.push_back(Data.size());
		}

		// 頂点数
		m_VertexCount = static_cast<int>(createInfo.GetVertices()[0].size() / createInfo.GetAttributeDimensions()[0]);

		return true;
	}

	bool CWebGPURenderer::CreateIndexBuffer(const CRendererCreateInfo& createInfo)
	{
		m_IndexCount = createInfo.GetIndices().size();

		if (!CreateBuffer(m_IndexBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index, &createInfo.GetIndices()[0], m_IndexCount * sizeof(uint16_t))) return false;

		return true;
	}

	bool CWebGPURenderer::CreateGraphicsPipeline(const CRendererCreateInfo& createInfo)
	{
		// シェーダーモジュールの生成 //////////////////////////////////////////////////////////////////
		WGPUShaderModule vertexShaderModele = CreateShaderModule(createInfo.GetVertexShaderCode());
		WGPUShaderModule fragmentShaderModele = CreateShaderModule(createInfo.GetFragmentShaderCode());

		// パイプラインの設定 //////////////////////////////////////////////////////////////////////////
		WGPURenderPipelineDescriptor pipelineDesc{};
		pipelineDesc.nextInChain = nullptr; // 拡張機能

		// 頂点バッファレイアウト
		std::vector<WGPUVertexBufferLayout> vertexBufferLayouts(m_VertexBufferList.size());
		std::vector<WGPUVertexAttribute> attributes(m_VertexBufferList.size()); // ここベクターにしないとなんかvertexBufferLayoutsに入れておいてもメモリが解放されててなんか数値がおかしなことに・・・
		// ↑↑↑ 確かにスタックメモリに格納する変数はスコープを抜けたら解放されるよね・・・
		// そしてその解放されたものを使用していると当然おかしくなる
		// メモリの解放タイミングと使用タイミングには留意しよう！

		for (int i = 0; i < static_cast<int>(m_VertexBufferList.size()); i++)
		{
			//
			int Dimension = createInfo.GetAttributeDimensions()[i];

			//
			attributes[i].shaderLocation = i; // Shaderでのアトリビュートインデックス
			attributes[i].format = GetVertexFormat(Dimension);
			attributes[i].offset = 0;

			//
			vertexBufferLayouts[i].attributeCount = 1;
			vertexBufferLayouts[i].attributes = &attributes[i];
			vertexBufferLayouts[i].arrayStride = Dimension * sizeof(float); // ストライドとは連続する要素間のバイト数のこと
			vertexBufferLayouts[i].stepMode = WGPUVertexStepMode_Vertex; // ??? 頂点データが同じインスタンスなら共有されることを示す設定 ???
		}

		// 頂点シェーダー
		pipelineDesc.vertex.bufferCount = static_cast<uint32_t>(vertexBufferLayouts.size()); // 頂点バッファ
		pipelineDesc.vertex.buffers = &vertexBufferLayouts[0];
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

		if (!m_GraphicsPipeline)
		{
			Console::Log("[Error] m_GraphicsPipeline is null\n");
			return false;
		}

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

	WGPUVertexFormat CWebGPURenderer::GetVertexFormat(int Dimension)
	{
		WGPUVertexFormat format;

		switch (Dimension)
		{
		case 1:
			format = WGPUVertexFormat_Float32;
			break;

		case 2:
			format = WGPUVertexFormat_Float32x2;
			break;

		case 3:
			format = WGPUVertexFormat_Float32x3;
			break;

		case 4:
			format = WGPUVertexFormat_Float32x4;
			break;
		default:
			break;
		}

		return format;
	}

	bool CWebGPURenderer::CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize)
	{
		WGPUBufferDescriptor bufferDesc{};
		bufferDesc.nextInChain = nullptr; // 拡張機能
		bufferDesc.usage = Usage; // バッファの用途
		bufferDesc.mappedAtCreation = false; // ???
		bufferDesc.size = ByteSize;

		Buffer = wgpuDeviceCreateBuffer(m_pGraphicsAPI->GetLogicalDevice(), &bufferDesc);

		// バッファにデータを書き込む
		wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), Buffer, 0, Data, bufferDesc.size);

		return true;
	}
}
#endif