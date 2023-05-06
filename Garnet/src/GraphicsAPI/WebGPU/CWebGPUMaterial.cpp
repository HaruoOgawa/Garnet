#ifdef __DAWN__
#include "CWebGPUMaterial.h"
#include "CWebGPUAPI.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"

namespace api
{
	CWebGPUMaterial::CWebGPUMaterial():
		CMaterial(),
		m_pGraphicsAPI(nullptr),
		m_VertexShaderModele(nullptr),
		m_FragmentShaderModele(nullptr),
		m_UniformBuffer(nullptr),
		m_UniformCount(0),
		m_BindGroupLayout(nullptr),
		m_BindGroup(nullptr)
	{
	}

	CWebGPUMaterial::~CWebGPUMaterial()
	{
		wgpuBufferDestroy(m_UniformBuffer);
	}

	bool CWebGPUMaterial::Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo)
	{
		m_pGraphicsAPI = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		if (!CreateShaderStages(createInfo)) return false;
		if (!CreateUniformBuffer(createInfo)) return false; // ユニフォームバッファを生成
		if (!CreateBindGroup(createInfo)) return false; // バインドグループを生成(レンダリングパイプラインで使用するすべてのリソースをどのようにバインドするかを指定するオブジェクト)

		return true;
	}

	bool CWebGPUMaterial::Update(float SecondsTime)
	{
		// ユニフォームバッファの更新
		float t = SecondsTime;
		glm::vec3 testPos = glm::vec3(0.0f);

		// 行列
		glm::mat4 mmat = glm::rotate(glm::mat4(1.0f), SecondsTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 vmat = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 pmat = glm::perspective(
			glm::radians(45.0f),
			1.0f, 0.1f, 10.0f
		);

		glm::mat4 mvp = pmat * vmat * mmat;

		//wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_UniformBuffer, 4 * sizeof(float), &t, sizeof(float));
		wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_UniformBuffer, 16 * 3 * sizeof(float), reinterpret_cast<const float*>(&mvp[0][0]), 16 * sizeof(float));

		return true;
	}

	// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
	bool CWebGPUMaterial::CreateShaderStages(const graphics::CMaterialCreateInfo& createInfo)
	{
		// シェーダーモジュールの生成 //////////////////////////////////////////////////////////////////
		if (createInfo.GetShaderType() == graphics::EShaderType::SPIRV)
		{
			const auto& VertexShaderData = createInfo.GetVertexShaderCode();
			m_VertexShaderModele = CreateShaderModuleFromSPIRV(VertexShaderData);

			const auto& FragmentShaderCode = createInfo.GetFragmentShaderCode();
			m_FragmentShaderModele = CreateShaderModuleFromSPIRV(FragmentShaderCode);
		}
		else if (createInfo.GetShaderType() == graphics::EShaderType::WGSL)
		{
			const auto& VertexShaderData = createInfo.GetVertexShaderCode();
			m_VertexShaderModele = CreateShaderModuleFromWGSL(std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

			const auto& FragmentShaderCode = createInfo.GetFragmentShaderCode();
			m_FragmentShaderModele = CreateShaderModuleFromWGSL(std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CWebGPUMaterial::CreateUniformBuffer(const graphics::CMaterialCreateInfo& createInfo)
	{
		// バッファの生成
		std::vector<float> Data;

		// 行列
		glm::mat4 mmat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)) * glm::mat4_cast(glm::quat(glm::vec3(0.0f))) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		glm::mat4 vmat = glm::lookAt(
			glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glm::mat4 pmat = glm::perspective(
			glm::radians(90.0f),
			1.0f,
			0.1f,
			10000.0f
		);

		glm::mat4 mvp = pmat * vmat * mmat;

		Data.resize(16 * 4);
		std::memcpy(&Data[16 * 0], reinterpret_cast<const float*>(&mmat[0][0]), 16 * sizeof(float));
		std::memcpy(&Data[16 * 1], reinterpret_cast<const float*>(&vmat[0][0]), 16 * sizeof(float));
		std::memcpy(&Data[16 * 2], reinterpret_cast<const float*>(&pmat[0][0]), 16 * sizeof(float));
		std::memcpy(&Data[16 * 3], reinterpret_cast<const float*>(&mvp[0][0]), 16 * sizeof(float));

		// 色
		std::vector<float> testUBO = {
			0.01f, 0.01f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f
		};

		Data.resize(Data.size() + testUBO.size());
		std::memcpy(&Data[16 * 4], &testUBO[0], sizeof(float) * testUBO.size());

		//
		m_UniformCount = Data.size();

		if (!CreateBuffer(m_UniformBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, &Data[0], m_UniformCount * sizeof(float))) return false;

		return true;
	}

	bool CWebGPUMaterial::CreateBindGroup(const graphics::CMaterialCreateInfo& createInfo)
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
		std::vector<WGPUBindGroupLayoutEntry> bindingLayoutList(2);

		{
			InitDefalutBindGroupLayoutEntry(bindingLayoutList[0]); // 初期化しないとブラウザ側でいろいろとエラーがでる・・・
			bindingLayoutList[0].binding = 0; // バインドインデックス
			bindingLayoutList[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment; // アクセス権限。ここではおそらく頂点シェーダーとフラグメントシェーダーのみ読み取り可
			bindingLayoutList[0].buffer.type = WGPUBufferBindingType_Uniform; // バインド先のバッファの種類
			int Stride = 16 * 4;
			bindingLayoutList[0].buffer.minBindingSize = Stride * sizeof(float); // データ一つ当たりのサイズかな???
		}

		{
			InitDefalutBindGroupLayoutEntry(bindingLayoutList[1]); // 初期化しないとブラウザ側でいろいろとエラーがでる・・・
			bindingLayoutList[1].binding = 1; // バインドインデックス
			bindingLayoutList[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment; // アクセス権限。ここではおそらく頂点シェーダーとフラグメントシェーダーのみ読み取り可
			bindingLayoutList[1].buffer.type = WGPUBufferBindingType_Uniform; // バインド先のバッファの種類
			int Stride = 4 * 4;
			bindingLayoutList[1].buffer.minBindingSize = Stride * sizeof(float); // データ一つ当たりのサイズかな???
		}

		// バインドグループレイアウトを作成
		// たぶん上記のバインドレイアウトのマネージャー, 複数個束ねるやつ
		// --> これがWGSLでいう @group(n) かな？
		WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{}; //バインドグループの記述子
		bindGroupLayoutDesc.nextInChain = nullptr; // 拡張機
		bindGroupLayoutDesc.label = "BindGroupLayout";
		bindGroupLayoutDesc.entryCount = static_cast<uint32_t>(bindingLayoutList.size()); // 上記のバインドレイアウトの数
		bindGroupLayoutDesc.entries = &bindingLayoutList[0]; // バインドレイアウトのデータ
		m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_pGraphicsAPI->GetLogicalDevice(), &bindGroupLayoutDesc);
		if (!m_BindGroupLayout)
		{
			Console::Log("[Error] BindGroupLayout is nullptr\n");
			return false;
		}

		// バッファとバインディングを結びつけるための記述かな？
		// --> その通り、たぶんバッファのバインディングとかバインディングのオフセットとか
		std::vector<WGPUBindGroupEntry> bindingList(2);

		{
			int Offset = 0;
			int Stride = 16 * 4;

			bindingList[0].nextInChain = nullptr; // 拡張機
			bindingList[0].binding = 0;
			bindingList[0].buffer = m_UniformBuffer;
			bindingList[0].offset = Offset;
			bindingList[0].size = Stride * sizeof(float);
		}

		{
			int Offset = 16 * 4 * sizeof(float);
			int Stride = 4 * 4;

			bindingList[1].nextInChain = nullptr; // 拡張機
			bindingList[1].binding = 1;
			bindingList[1].buffer = m_UniformBuffer;
			bindingList[1].offset = Offset;
			bindingList[1].size = Stride * sizeof(float);
		}

		// バインドグループを作成
		// --> groupやbindingやbufferなどのをすべてを最終的に束ねるためのもの
		WGPUBindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.nextInChain = nullptr; // 拡張機
		bindGroupDesc.label = "BindGroup";
		bindGroupDesc.layout = m_BindGroupLayout; // バインドグループレイアウト
		bindGroupDesc.entryCount = static_cast<uint32_t>(bindingList.size());
		bindGroupDesc.entries = &bindingList[0];
		m_BindGroup = wgpuDeviceCreateBindGroup(m_pGraphicsAPI->GetLogicalDevice(), &bindGroupDesc);

		if (!m_BindGroup)
		{
			Console::Log("[Error] BindGroup is nullptr\n");
			return false;
		}

		return true;
	}

	// Helper Function ///////////////////////////////////////////////////////////////////////
	WGPUShaderModule CWebGPUMaterial::CreateShaderModuleFromWGSL(const std::string& shaderCode)
	{
		//
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

	WGPUShaderModule CWebGPUMaterial::CreateShaderModuleFromSPIRV(const std::vector<char>& shaderCode)
	{
		std::vector<uint32_t> Data;
		Data.resize(shaderCode.size() / 4);
		std::memcpy(&Data[0], &shaderCode[0], shaderCode.size() * sizeof(char));

		WGPUShaderModuleSPIRVDescriptor shaderCodeDesc{};
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleSPIRVDescriptor;
		shaderCodeDesc.code = &Data[0];
		shaderCodeDesc.codeSize = static_cast<uint32_t>(Data.size());

		WGPUShaderModuleDescriptor shaderDesc{};
		shaderDesc.hintCount = 0;
		shaderDesc.hints = nullptr;
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &shaderDesc);

		return shaderModule;
	}

	bool CWebGPUMaterial::CreateBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize)
	{
		// たぶんWebGPU, Vulkanでもvec3は16バイトオフセットと換算されるっぽいからvec3分(12バイト分)のパディングを入れたい場合はvec3ではなくfloatの変数を3つ定義するべき

		WGPUBufferDescriptor bufferDesc{};
		bufferDesc.nextInChain = nullptr; // 拡張機
		bufferDesc.label = "Buffer";
		bufferDesc.usage = Usage; // バッファの用途
		bufferDesc.mappedAtCreation = false; // ???
		bufferDesc.size = ByteSize;

		Buffer = wgpuDeviceCreateBuffer(m_pGraphicsAPI->GetLogicalDevice(), &bufferDesc);

		// バッファにデータを書き込む
		wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), Buffer, 0, Data, bufferDesc.size);

		return true;
	}

	void CWebGPUMaterial::InitDefalutBindGroupLayoutEntry(WGPUBindGroupLayoutEntry& bindingLayout)
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