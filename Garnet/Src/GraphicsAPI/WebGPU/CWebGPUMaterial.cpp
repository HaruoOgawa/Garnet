#ifdef USE_WEBGPU
#include "CWebGPUMaterial.h"
#include "CWebGPUAPI.h"
#include "CWebGPUTexture.h"
#include "../CMaterialCreateInfo.h"
#include "../../Message/Console.h"
#include "../../Camera/CCamera.h"
#include "../../Projection/CProjection.h"

namespace api
{
	CWebGPUMaterial::CWebGPUMaterial(api::CWebGPUAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode):
		CMaterial(pGraphicsAPI, createInfo, CullMode),
		m_pGraphicsAPI(pGraphicsAPI),
		m_VertexShaderModele(nullptr),
		m_FragmentShaderModele(nullptr),
		m_ComputeShaderModele(nullptr),
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

	bool CWebGPUMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		// 参照テクスチャリスト
		if (!CreateRefTextureList(m_CreateInfo, TextureSet)) return false;

		if (!CreateShaderStages(m_CreateInfo)) return false;
		if (!CreateShaderBuffers(m_CreateInfo)) return false; // ユニフォームバッファを生成
		if (!CreateBindGroup(m_CreateInfo, TextureSet)) return false; // バインドグループを生成(レンダリングパイプラインで使用するすべてのリソースをどのようにバインドするかを指定するオブジェクト)

		return true;
	}

	bool CWebGPUMaterial::BuildDrawBuffer()
	{
		return true;
	}

	void CWebGPUMaterial::SetUniformValue(const std::string Name, const void* Data, int ByteSize)
	{
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			if (i >= m_WGPUUniformBufferByteSizeList.size()) continue;

			// SharedBufferは処理しない
			if (m_ShaderBufferList[i]->GetSharedBufferParam().IsShared) continue;

			auto& UniformBuffer = m_ShaderBufferList[i];
			auto UniformBufferByteSize = static_cast<uint64_t>(m_WGPUUniformBufferByteSizeList[i]);
			const auto& UniformDesc = UniformBuffer->GetDescriptor();

			const auto& DataList = UniformDesc.GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int ByteOffset = UniformData->second.ByteOffset;

				// API側のBufferを更新
				if (IsUseDynamicOffset())
				{
					int DynamicOffset = 0;

					const auto it = m_PassNameDynamicOffsetMap.find(m_pGraphicsAPI->GetCurrentRenderPassName());
					if (it != m_PassNameDynamicOffsetMap.end())
					{
						DynamicOffset = it->second;
					}

					wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_WGPUUniformBufferList[i], ByteOffset + UniformBufferByteSize * DynamicOffset, Data, ByteSize);
				}
				else
				{
					wgpuQueueWriteBuffer(m_pGraphicsAPI->GetQueue(), m_WGPUUniformBufferList[i], ByteOffset, Data, ByteSize);
				}

				// CPU側のBufferを更新
				UniformBuffer->SetValue(Data, ByteOffset, ByteSize);
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
			if (!VertexShaderData.empty()) m_VertexShaderModele = CreateShaderModuleFromSPIRV(VertexShaderData);

			const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
			if(!FragmentShaderCode.empty()) m_FragmentShaderModele = CreateShaderModuleFromSPIRV(FragmentShaderCode);
			
			const auto& ComputeShaderCode = createInfo->GetComputeShaderCode();
			if(!ComputeShaderCode.empty()) m_ComputeShaderModele = CreateShaderModuleFromSPIRV(ComputeShaderCode);
		}
		else if (createInfo->GetShaderType() == graphics::EShaderType::WGSL)
		{
			const auto& VertexShaderData = createInfo->GetVertexShaderCode();
			if (!VertexShaderData.empty()) m_VertexShaderModele = CreateShaderModuleFromWGSL(std::string(&VertexShaderData[0], &VertexShaderData[0] + VertexShaderData.size()));

			const auto& FragmentShaderCode = createInfo->GetFragmentShaderCode();
			if (!FragmentShaderCode.empty()) m_FragmentShaderModele = CreateShaderModuleFromWGSL(std::string(&FragmentShaderCode[0], &FragmentShaderCode[0] + FragmentShaderCode.size()));
			
			const auto& ComputeShaderCode = createInfo->GetComputeShaderCode();
			if (!ComputeShaderCode.empty()) m_ComputeShaderModele = CreateShaderModuleFromWGSL(std::string(&ComputeShaderCode[0], &ComputeShaderCode[0] + ComputeShaderCode.size()));
		}
		else
		{
			return false;
		}

		return true;
	}

	bool CWebGPUMaterial::CreateShaderBuffers(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		for (const auto& Buffer : m_ShaderBufferList)
		{
			// SharedBufferは処理しない
			if (Buffer->GetSharedBufferParam().IsShared) continue;

			const auto& Data = Buffer->GetBuffer();

			WGPUBuffer UniformBuffer;
			const uint64_t ByteSize = static_cast<uint64_t>(Data.size());

			// DynamicOffsetはバッファサイズが256バイト以上でないと使用できないので使用する設定になっていてそれよりも小さい時はエラーとする
			if (IsUseDynamicOffset() && ByteSize < 256)
			{
				Console::Log("[API Error] ByteSize must be rather than 256 byte if use DynamicOffset. / BufferName: %s / ByteSize: %d\n", Buffer->GetBufferName().c_str(), static_cast<int>(ByteSize));

				return false;
			}

			if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
			{
				if (!CreateWGUniformBuffer(UniformBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, &Data[0], ByteSize)) return false;
			}
			else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
			{
				if (!CreateWGUniformBuffer(UniformBuffer, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage, &Data[0], ByteSize)) return false;
			}

			m_WGPUUniformBufferList.push_back(UniformBuffer);
			m_WGPUUniformBufferByteSizeList.push_back(static_cast<uint32_t>(ByteSize));
		}

		return true;
	}

	bool CWebGPUMaterial::CreateBindGroup(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		//
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList(0);
		if (TextureSet) TextureList = TextureSet->Get2DTextureList();

		std::vector<std::shared_ptr<graphics::CTexture>> CubeMapList(0);
		if (TextureSet) CubeMapList = TextureSet->GetCubeMapList();

		std::vector<std::shared_ptr<graphics::CTexture>> FrameTextureList(0);
		if (TextureSet) FrameTextureList = TextureSet->GetFrameTextureList();

		std::shared_ptr<graphics::CTexture> Diffuse_Tex = nullptr;
		if(TextureSet) Diffuse_Tex = TextureSet->GetDiffuse_Tex();

		std::shared_ptr<graphics::CTexture> Specular_Tex = nullptr;
		if (TextureSet) Specular_Tex = TextureSet->GetSpecular_Tex();

		std::shared_ptr<graphics::CTexture> GGXLUT_Tex = nullptr;
		if (TextureSet) GGXLUT_Tex = TextureSet->GetGGXLUT_Tex();

		// バインドレイアウトを作成
		// どのようにメモリに配置されるか, バインドインデックスや読み取り専用かなど
		// -->これがWGSLでいう @binding(n)
		std::vector<WGPUBindGroupLayoutEntry> bindingLayoutList;

		// UBO
		for (const auto& Buffer : m_ShaderBufferList)
		{
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				WGPUBindGroupLayoutEntry bindingLayout{};
				InitDefalutBindGroupLayoutEntry(bindingLayout); // 初期化しないとブラウザ側でいろいろとエラーがでる・・・
				bindingLayout.binding = Layout.second.BindingIndex; // バインドインデックス
				
				if (Buffer->GetBufferType() == graphics::EBufferType::UNIFORM)
				{
					bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute; // アクセス権限。ここではおそらく頂点シェーダーとフラグメントシェーダーのみ読み取り可
					bindingLayout.buffer.type = WGPUBufferBindingType_Uniform; // バインド先のバッファの種類
				}
				else if (Buffer->GetBufferType() == graphics::EBufferType::SHADERSTORAGE)
				{
					if (Layout.second.IsGPGPUWritable) // 読み書き可能なGPGPU用のバッファ
					{
						// WGPUBufferBindingType_StorageはWGPUShaderStage_Computeだけに割り当てることができる
						bindingLayout.visibility = WGPUShaderStage_Compute;
						bindingLayout.buffer.type = WGPUBufferBindingType_Storage; 
					}
					else
					{
						bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
						bindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage; // 基本的にStorage BufferはReadOnly. ComputeのみRead_Writeが使える
					}
				}

				bindingLayout.buffer.minBindingSize = Layout.second.ByteSize; // データ一つ当たりのサイズかな???
				bindingLayout.buffer.hasDynamicOffset = (IsUseDynamicOffset()); // ダイナミックユニフォーム
				
				bindingLayoutList.push_back(bindingLayout);
			}
		}

		// Texture
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
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

			{
				WGPUBindGroupLayoutEntry bindingLayout{};
				InitDefalutBindGroupLayoutEntry(bindingLayout);
				bindingLayout.binding = TexLayout.ViewBindingIndex;
				bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
				bindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
				bindingLayout.texture.viewDimension = (Texture->GetTextureType() == graphics::ETextureType::TEXTURE_CUBE) ? WGPUTextureViewDimension_Cube : WGPUTextureViewDimension_2D;

				bindingLayoutList.push_back(bindingLayout);
			}

			{
				WGPUBindGroupLayoutEntry bindingLayout{};
				InitDefalutBindGroupLayoutEntry(bindingLayout);
				bindingLayout.binding = TexLayout.SamplerBindingIndex;
				bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
				bindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

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
		// UBO
		std::vector<WGPUBindGroupEntry> bindingList;
		for (int i = 0; i < m_ShaderBufferList.size(); i++)
		{
			// 共有バッファ
			const auto& SharedBufferParam = m_ShaderBufferList[i]->GetSharedBufferParam();

			const auto& Buffer = m_ShaderBufferList[i];
			for (const auto& Layout : Buffer->GetBindingLayoutList())
			{
				WGPUBindGroupEntry binding{};
				
				binding.nextInChain = nullptr; // 拡張機
				binding.binding = Layout.second.BindingIndex;

				if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
				{
					CWebGPUMaterial* pSharedWebGPUMat = static_cast<CWebGPUMaterial*>(SharedBufferParam.SharedBufferMaterial.get());

					binding.buffer = pSharedWebGPUMat->GetWGPUUniformBufferList()[SharedBufferParam.BufferIndex];
				}
				else // 通常のバッファ使用
				{
					binding.buffer = m_WGPUUniformBufferList[i];
				}
				
				binding.offset = Layout.second.ByteOffset;
				binding.size = Layout.second.ByteSize;

				bindingList.push_back(binding);

				if (SharedBufferParam.IsShared) // バッファを他のマテリアルと共有する
				{
					CWebGPUMaterial* pSharedWebGPUMat = static_cast<CWebGPUMaterial*>(SharedBufferParam.SharedBufferMaterial.get());

					m_BindingRefSizeList.push_back(pSharedWebGPUMat->GetWGPUUniformBufferByteSizeList()[SharedBufferParam.BufferIndex]);
				}
				else
				{
					m_BindingRefSizeList.push_back(m_WGPUUniformBufferByteSizeList[i]);
				}
				
			}
		}

		// Texture
		for (const auto& TexLayout : m_TextureBindingLayoutList)
		{
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

			api::CWebGPUTexture* pWebGPUTexture = static_cast<api::CWebGPUTexture*>(Texture.get());

			{
				WGPUBindGroupEntry binding{};
				binding.nextInChain = nullptr;
				binding.binding = TexLayout.ViewBindingIndex;
				binding.textureView = pWebGPUTexture->GetTextureImageView();

				bindingList.push_back(binding);
			}

			{
				WGPUBindGroupEntry binding{};
				binding.nextInChain = nullptr;
				binding.binding = TexLayout.SamplerBindingIndex;
				binding.sampler = pWebGPUTexture->GetTextureSampler();

				bindingList.push_back(binding);
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
		shaderDesc.nextInChain = &shaderCodeDesc.chain; // 使用するShaderの種類を指定(ここででWGSL)。WGPUShaderModuleSPIRVDescriptorを指定することでVulkanのSplivが使用できる(SplivはGLSLからコンパイルできて便利)

		WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &shaderDesc);

		return shaderModule;
	}

	WGPUShaderModule CWebGPUMaterial::CreateShaderModuleFromSPIRV(const std::vector<unsigned char>& shaderCode)
	{
		std::vector<uint32_t> Data;
		Data.resize(shaderCode.size() / 4);
		std::memcpy(&Data[0], &shaderCode[0], shaderCode.size() * sizeof(unsigned char));

		WGPUShaderModuleSPIRVDescriptor shaderCodeDesc{};
		shaderCodeDesc.chain.next = nullptr;
		shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleSPIRVDescriptor;
		shaderCodeDesc.code = &Data[0];
		shaderCodeDesc.codeSize = static_cast<uint32_t>(Data.size());

		WGPUShaderModuleDescriptor shaderDesc{};
		shaderDesc.nextInChain = &shaderCodeDesc.chain;

		WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(m_pGraphicsAPI->GetLogicalDevice(), &shaderDesc);

		return shaderModule;
	}

	bool CWebGPUMaterial::CreateWGUniformBuffer(WGPUBuffer& Buffer, WGPUBufferUsageFlags Usage, void const* Data, uint64_t ByteSize)
	{
		// たぶんWebGPU, Vulkanでもvec3は16バイトオフセットと換算されるっぽいからvec3分(12バイト分)のパディングを入れたい場合はvec3ではなくfloatの変数を3つ定義するべき

		const int RefCount = static_cast<int>(m_PassNameDynamicOffsetMap.size());

		WGPUBufferDescriptor bufferDesc{};
		bufferDesc.nextInChain = nullptr; // 拡張機
		bufferDesc.label = "Buffer";
		bufferDesc.usage = Usage; // バッファの用途
		bufferDesc.mappedAtCreation = false; // ???
		bufferDesc.size = ByteSize * RefCount;

		Buffer = wgpuDeviceCreateBuffer(m_pGraphicsAPI->GetLogicalDevice(), &bufferDesc);

		// バッファにデータを書き込む
		if (IsUseDynamicOffset())
		{
			for (int i = 0; i < RefCount; i++)
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