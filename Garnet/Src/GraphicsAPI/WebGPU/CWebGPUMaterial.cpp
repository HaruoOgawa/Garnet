#ifdef __DAWN__
#include "CWebGPUMaterial.h"
#include "CWebGPUAPI.h"
#include "../CMaterialCreateInfo.h"
#include "../../Debug/Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"
#include "../../Math/CMath.h"

namespace api
{
	CWebGPUMaterial::CWebGPUMaterial(api::CWebGPUAPI* pGraphicsAPI):
		CMaterial(),
		m_pGraphicsAPI(pGraphicsAPI),
		m_VertexShaderModele(nullptr),
		m_FragmentShaderModele(nullptr),
		m_BindGroupLayout(nullptr),
		m_BindGroup(nullptr)
	{
	}

	CWebGPUMaterial::~CWebGPUMaterial()
	{
		for (auto& UniformBuffer : m_WGPUUniformBufferList)
		{
			wgpuBufferDestroy(UniformBuffer);
		}
		m_WGPUUniformBufferList.clear();
	}

	bool CWebGPUMaterial::Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList)
	{
		if (!CreateShaderStages(m_CreateInfo)) return false;
		if (!CreateUniformBuffer(m_CreateInfo)) return false; // ユニフォームバッファを生成
		if (!CreateBindGroup(m_CreateInfo)) return false; // バインドグループを生成(レンダリングパイプラインで使用するすべてのリソースをどのようにバインドするかを指定するオブジェクト)

		// 生成処理が終わったので不要なリソースを解放する
		m_CreateInfo = nullptr;

		return true;
	}

	bool CWebGPUMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		// 共通のユニフォームバッファの更新
		SetUniformValue("view", &Camera->GetViewMatrix()[0][0], -1);
		SetUniformValue("proj", &Projection->GetPrejectionMatrix()[0][0], -1);

		return true;
	}

	bool CWebGPUMaterial::BuildDrawBuffer(int DynamicOffsetNum)
	{
		return true;
	}

	void CWebGPUMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			auto& UniformBuffer = m_UniformBufferList[i];
			auto UniformBufferByteSize = static_cast<uint64_t>(m_WGPUUniformBufferByteSizeList[i]);
			const auto& UniformDesc = UniformBuffer->GetDescriptor();

			const auto& DataList = UniformDesc->GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int ByteOffset = UniformData->second.ByteOffset;
				const int ByteSize = UniformData->second.ByteSize;

				if (m_UseDynamicUniform)
				{
					if (DynamicOffsetNum == -1)
					{
						for (int r = 0; r < m_RefCount; r++)
						{
							wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_WGPUUniformBufferList[i], ByteOffset + UniformBufferByteSize * r, Value, ByteSize);
						}
					}
					else
					{
						wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_WGPUUniformBufferList[i], ByteOffset + UniformBufferByteSize * (DynamicOffsetNum - 1), Value, ByteSize);
					}
				}
				else
				{
					wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_WGPUUniformBufferList[i], ByteOffset, Value, ByteSize);
				}
			}
		}
	}

	// WebGPU Main Logic /////////////////////////////////////////////////////////////////////
	bool CWebGPUMaterial::CreateShaderStages(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		// シェーダーモジュールの生成 //////////////////////////////////////////////////////////////////
		if (createInfo->GetShaderType() == graphics::EShaderType::SPIRV)
		{
			const auto& VertexShaderData = createInfo->GetVertexShaderCode();
			m_VertexShaderModele = CreateShaderModuleFromSPIRV(VertexShaderData);

			const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
			m_FragmentShaderModele = CreateShaderModuleFromSPIRV(FragmentShaderCode);
		}
		else if (createInfo->GetShaderType() == graphics::EShaderType::WGSL)
		{
			const auto& VertexShaderData = createInfo->GetVertexShaderCode();
			m_VertexShaderModele = CreateShaderModuleFromWGSL(std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

			const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
			m_FragmentShaderModele = CreateShaderModuleFromWGSL(std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CWebGPUMaterial::CreateUniformBuffer(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		for (const auto& Buffer : m_UniformBufferList)
		{
			const auto& Data = Buffer->GetData();

			WGPUBuffer UniformBuffer;
			const uint64_t ByteSize = static_cast<uint64_t>(math::GetNextPowerOfTwo(static_cast<unsigned int>(Data.size()))); // 2のn乗にする

			if (!CreateWGUniformBuffer(UniformBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, &Data[0], ByteSize)) return false;

			m_WGPUUniformBufferList.push_back(UniformBuffer);
			m_WGPUUniformBufferByteSizeList.push_back(static_cast<uint32_t>(ByteSize));
		}

		return true;
	}

	bool CWebGPUMaterial::CreateBindGroup(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		// バインドレイアウトを作成
		// どのようにメモリに配置されるか, バインドインデックスや読み取り専用かなど
		// -->これがWGSLでいう @binding(n)
		std::vector<WGPUBindGroupLayoutEntry> bindingLayoutList;

		for (const auto& Buffer : m_UniformBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				WGPUBindGroupLayoutEntry bindingLayout{};
				InitDefalutBindGroupLayoutEntry(bindingLayout); // 初期化しないとブラウザ側でいろいろとエラーがでる・・・
				bindingLayout.binding = Layout.BindingIndex; // バインドインデックス
				bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment; // アクセス権限。ここではおそらく頂点シェーダーとフラグメントシェーダーのみ読み取り可
				bindingLayout.buffer.type = WGPUBufferBindingType_Uniform; // バインド先のバッファの種類
				bindingLayout.buffer.minBindingSize = Layout.ByteSize; // データ一つ当たりのサイズかな???
				bindingLayout.buffer.hasDynamicOffset = true; // ダイナミックユニフォーム

				bindingLayoutList.push_back(bindingLayout);
			}
		}

		// バインドグループレイアウトを作成
		// たぶん上記のバインドレイアウトのマネージャー, 複数個束ねるやつ
		// --> これがWGSLでいう @group(n) かな？
		WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{}; //バインドグループの記述子
		bindGroupLayoutDesc.nextInChain = nullptr; // 拡張機
		bindGroupLayoutDesc.label = "BindGroupLayout";
		bindGroupLayoutDesc.entryCount = static_cast<uint32_t>(bindingLayoutList.size()); // 上記のバインドレイアウトの数
		bindGroupLayoutDesc.entries = bindingLayoutList.data(); // バインドレイアウトのデータ
		m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_pGraphicsAPI->GetLogicalDevice(), &bindGroupLayoutDesc);
		if (!m_BindGroupLayout)
		{
			Console::Log("[Error] BindGroupLayout is nullptr\n");
			return false;
		}

		// バッファとバインディングを結びつけるための記述かな？
		// --> その通り、たぶんバッファのバインディングとかバインディングのオフセットとか
		std::vector<WGPUBindGroupEntry> bindingList;
		for (int i = 0; i < m_UniformBufferList.size(); i++)
		{
			const auto& Buffer = m_UniformBufferList[i];
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				WGPUBindGroupEntry binding{};
				int Offset = 0;
				int Stride = 16 * 4;

				binding.nextInChain = nullptr; // 拡張機
				binding.binding = Layout.BindingIndex;
				binding.buffer = m_WGPUUniformBufferList[i];
				binding.offset = Layout.ByteOffset;
				binding.size = Layout.ByteSize;

				bindingList.push_back(binding);
				m_BindingRefSizeList.push_back(m_WGPUUniformBufferByteSizeList[i]);
			}
		}

		// バインドグループを作成
		// --> groupやbindingやbufferなどのをすべてを最終的に束ねるためのもの
		WGPUBindGroupDescriptor bindGroupDesc{};
		bindGroupDesc.nextInChain = nullptr; // 拡張機
		bindGroupDesc.label = "BindGroup";
		bindGroupDesc.layout = m_BindGroupLayout; // バインドグループレイアウト
		bindGroupDesc.entryCount = static_cast<uint32_t>(bindingList.size());
		bindGroupDesc.entries = bindingList.data();
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

	bool CWebGPUMaterial::CreateWGUniformBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize)
	{
		// たぶんWebGPU, Vulkanでもvec3は16バイトオフセットと換算されるっぽいからvec3分(12バイト分)のパディングを入れたい場合はvec3ではなくfloatの変数を3つ定義するべき

		WGPUBufferDescriptor bufferDesc{};
		bufferDesc.nextInChain = nullptr; // 拡張機
		bufferDesc.label = "Buffer";
		bufferDesc.usage = Usage; // バッファの用途
		bufferDesc.mappedAtCreation = false; // ???
		bufferDesc.size = ByteSize * ((m_UseDynamicUniform) ? m_RefCount : 1);

		Buffer = wgpuDeviceCreateBuffer(m_pGraphicsAPI->GetLogicalDevice(), &bufferDesc);

		// バッファにデータを書き込む
		if (m_UseDynamicUniform)
		{
			for (int i = 0; i < m_RefCount; i++)
			{
				wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), Buffer, ByteSize * i, Data, ByteSize);
			}
		}
		else
		{
			wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), Buffer, 0, Data, ByteSize);
		}

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