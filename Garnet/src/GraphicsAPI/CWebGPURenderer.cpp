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
		m_IndexCount(0),
		m_UniformBuffer(nullptr),
		m_UniformCount(0),
		m_BindGroup(nullptr)
	{
	}

	CWebGPURenderer::~CWebGPURenderer()
	{
		wgpuBufferDestroy(m_UniformBuffer);
		wgpuBufferDestroy(m_IndexBuffer);
		for (auto& Buffer : m_VertexBufferList)
		{
			wgpuBufferDestroy(Buffer);
		}
	}

	bool CWebGPURenderer::Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		if (!CreateVertexBuffer(createInfo)) return false; // 頂点バッファを生成
		if (!CreateIndexBuffer(createInfo)) return false; //インデックスバッファを生成
		if (!CreateUniformBuffer(createInfo)) return false; // ユニフォームバッファを生成
		if (!CreateBindGroup(createInfo)) return false; // バインドグループを生成(レンダリングパイプラインで使用するすべてのリソースをどのようにバインドするかを指定するオブジェクト)
		if (!CreateGraphicsPipeline(createInfo)) return false; // グラフィックスパイプラインを生成
		
		return true;
	}

	bool CWebGPURenderer::Update()
	{
		// ユニフォームバッファの更新
		//float t = static_cast<float>(glfwGetTime()); // glfwGetTime returns a double
		float t = 0.0f;
		glm::vec3 testPos = glm::vec3(0.0f);
		wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_UniformBuffer, 4 * sizeof(float), &testPos.x, sizeof(float));

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

		// バインドグループを割り当てる
		wgpuRenderPassEncoderSetBindGroup(m_pGraphicsAPI->GetRenderPass(), 0, m_BindGroup, 0, nullptr);

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

	bool CWebGPURenderer::CreateUniformBuffer(const CRendererCreateInfo& createInfo)
	{
		// バッファの生成
		std::vector<float> Data = {
			0.0f, 1.0f, 0.4f, 1.0f,
			0.0f,
			0.0f, 0.0f, 0.0f
		};

		m_UniformCount = Data.size();
	
		if (!CreateBuffer(m_UniformBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, &Data[0], m_UniformCount * sizeof(float))) return false;

		return true;
	}

	bool CWebGPURenderer::CreateBindGroup(const CRendererCreateInfo& createInfo)
	{
		// WGSLでユニフォームを渡す場所は、groupを大枠にその中にbindingしていく
		// @group(0) @binding(0) var<uniform> a: f32;
		// @group(0) @binding(1) var<uniform> b: f32;
		//
		// @group(1) @binding(0) var<uniform> c: f32;
		// @group(1) @binding(1) var<uniform> d: f32;
		// @group(1) @binding(2) var<uniform> e: f32;
		// なのでこのバインディング要素とグループを下記で作成する必要がある

		// バインドレイアウトを作成
		// どのようにメモリに配置されるか, バインドインデックスや読み取り専用かなど
		// -->これがWGSLでいう @binding(n)
		WGPUBindGroupLayoutEntry bindingLayout{};
		InitDefalutBindGroupLayoutEntry(bindingLayout); // 初期化しないとブラウザ側でいろいろとエラーがでる・・・
		bindingLayout.binding = 0; // バインドインデックス
		bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment; // アクセス権限。ここではおそらく頂点シェーダーとフラグメントシェーダーのみ読み取り可能
		bindingLayout.buffer.type = WGPUBufferBindingType_Uniform; // バインド先のバッファの種類
		bindingLayout.buffer.minBindingSize = m_UniformCount * sizeof(float); // データ一つ当たりのサイズかな???

		// バインドグループレイアウトを作成
		// たぶん上記のバインドレイアウトのマネージャー, 複数個束ねるやつ
		// --> これがWGSLでいう @group(n) かな？
		WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{}; //バインドグループの記述子
		bindGroupLayoutDesc.nextInChain = nullptr; // 拡張機能
		bindGroupLayoutDesc.label = "BindGroupLayout";
		bindGroupLayoutDesc.entryCount = 1; // 上記のバインドレイアウトの数
		bindGroupLayoutDesc.entries = &bindingLayout; // バインドレイアウトのデータ
		m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_pGraphicsAPI->GetLogicalDevice(), &bindGroupLayoutDesc);

		// バッファと結びつけるための記述かな？
		// --> その通り、たぶんバッファのバインディング
		WGPUBindGroupEntry binding{};
		binding.nextInChain = nullptr; // 拡張機能
		binding.binding = 0;
		binding.buffer = m_UniformBuffer;
		binding.offset = 0;
		binding.size = m_UniformCount * sizeof(float);

		// バインドグループを作成
		// --> groupやbindingやbufferなどのをすべてを最終的に束ねるためのもの
		WGPUBindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.nextInChain = nullptr; // 拡張機能
		bindGroupDesc.label = "BindGroup";
		bindGroupDesc.layout = m_BindGroupLayout; // バインドグループレイアウト
		bindGroupDesc.entryCount = bindGroupLayoutDesc.entryCount;
		bindGroupDesc.entries = &binding;
		m_BindGroup = wgpuDeviceCreateBindGroup(m_pGraphicsAPI->GetLogicalDevice(), &bindGroupDesc);

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
		
		// パイプラインレイアウトの指定
		// パイプラインレイアウトは、レンダリングパイプラインで使用されるすべてのリソースをどのようにバインドする必要があるかを示す
		WGPUPipelineLayoutDescriptor layoutDesc{};
		layoutDesc.nextInChain = nullptr;
		layoutDesc.bindGroupLayoutCount = 1;
		layoutDesc.bindGroupLayouts = &m_BindGroupLayout;
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
		// たぶんWebGPU, Vulkanでもvec3は16バイトオフセットと換算されるっぽいからvec3分(12バイト分)のパディングを入れたい場合はvec3ではなくfloatの変数を3つ定義するべき

		WGPUBufferDescriptor bufferDesc{};
		bufferDesc.nextInChain = nullptr; // 拡張機能
		bufferDesc.label = "Buffer";
		bufferDesc.usage = Usage; // バッファの用途
		bufferDesc.mappedAtCreation = false; // ???
		bufferDesc.size = ByteSize;

		Buffer = wgpuDeviceCreateBuffer(m_pGraphicsAPI->GetLogicalDevice(), &bufferDesc);

		// バッファにデータを書き込む
		wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), Buffer, 0, Data, bufferDesc.size);

		return true;
	}

	void CWebGPURenderer::InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout)
	{
		bindingLayout.buffer.nextInChain = nullptr;
		bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
		bindingLayout.buffer.hasDynamicOffset = false;

		bindingLayout.sampler.nextInChain = nullptr;
		bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

		bindingLayout.storageTexture.nextInChain = nullptr;
		bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
		bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
		bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

		bindingLayout.texture.nextInChain = nullptr;
		bindingLayout.texture.multisampled = false;
		bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
		bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
	}
}
#endif