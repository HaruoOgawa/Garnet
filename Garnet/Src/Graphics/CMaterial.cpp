#include "CMaterial.h"
#include "CMaterialFrame.h"

namespace graphics
{
	CMaterial::CMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterialCreateInfo>& createInfo, int RefCount, ECullMode CullMode):
		timeline::CTimelineTrackContent(),
		m_MaterialName(std::string()),
		m_MaterialFrame(nullptr),
		m_CreateInfo(createInfo),
		m_RefCount(RefCount),
		m_CurrentDynamicOffset(0),
		m_DepthMaterial(nullptr),
		m_EnabledZWrite(true),
		m_DepthFunc(EDepthFunc::Less),
		m_DefaultCullMode(CullMode),
		m_CullMode(CullMode),
		m_BlendType(EBlendType::BLEND_TYPE_ADDITIVE),
		m_IsDrawOutline(false),
		m_EmptyTexture(nullptr),
		m_EmptyCubeTexture(nullptr),
		m_OutputColorCount(1)
	{
		ResetDynamicOffset();

		{
			
			m_EmptyTexture = pGraphicsAPI->CreateTexture(false);
			std::vector<unsigned char> emptyPixel = { 0, 0, 0, 0 };
			m_EmptyTexture->Create(emptyPixel, static_cast<int>(emptyPixel.size() * sizeof(unsigned char)));
		}

		{
			m_EmptyCubeTexture = pGraphicsAPI->CreateTexture(false);
			m_EmptyCubeTexture->SetTextureType(graphics::ETextureType::TEXTURE_CUBE);
			std::vector<unsigned char> emptyCubePixel;
			for (int i = 0; i < 4 * 6; i++) { emptyCubePixel.push_back(0); }
			m_EmptyCubeTexture->Create(emptyCubePixel, static_cast<int>(emptyCubePixel.size() * sizeof(unsigned char)));
		}
	}

	bool CMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		return true;
	}

	bool CMaterial::IsUseShaderBuffer()
	{
		return (m_ShaderBufferList.size() > 0 || m_TextureBindingLayoutList.size() > 0);
	}

	void CMaterial::SetMaterialName(const std::string& Name)
	{
		m_MaterialName = Name;
	}

	const std::string& CMaterial::GetMaterialName() const
	{
		return m_MaterialName;
	}

	// カラーバッファへのアウトプット数(MRTで使用)
	void CMaterial::SetOutputColorCount(int Val)
	{
		m_OutputColorCount = Val;
	}

	int CMaterial::GetOutputColorCount() const
	{
		return m_OutputColorCount;
	}

	const std::shared_ptr<CMaterialFrame>& CMaterial::GetMaterialFrame() const
	{
		return m_MaterialFrame;
	}

	void CMaterial::SetMaterialFrame(const std::shared_ptr<CMaterialFrame>& MaterialFrame)
	{
		m_MaterialFrame = MaterialFrame;
	}

	bool CMaterial::DeleteMaterialFrameReference()
	{
		if (!m_MaterialFrame) return false;

		return m_MaterialFrame->DeleteRefMaterial(shared_from_this());
	}

	std::vector<std::shared_ptr<CShaderBuffer>>& CMaterial::GetShaderBufferList()
	{
		return m_ShaderBufferList;
	}

	const std::vector<STextureBindingLayout>& CMaterial::GetTextureBindingLayoutList() const
	{
		return m_TextureBindingLayoutList;
	}

	void CMaterial::SetTextureBindingLayoutTextureIndex(int BindingLayoutIndex, int TextureIndex, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		if (BindingLayoutIndex < 0 || BindingLayoutIndex >= static_cast<int>(m_TextureBindingLayoutList.size())) return;

		m_TextureBindingLayoutList[BindingLayoutIndex].TextureIndex = TextureIndex;

		CreateRefTextureList(m_CreateInfo, TextureSet);
	}

	bool CMaterial::CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF)
	{
		m_DepthMaterial = DepthMF->CreateMaterial(pGraphicsAPI, m_RefCount, m_CullMode);

		m_DepthMaterial->SetCullMode(graphics::ECullMode::CULL_FRONT);

		if (!m_DepthMaterial->Create(nullptr)) return false;

		return true;
	}

	bool CMaterial::ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<CShaderBuffer>>& ShaderBufferList, const std::vector<STextureBindingLayout>& TextureBindingLayoutList)
	{
		return true;
	}

	bool CMaterial::ReCreateBuffer(const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList)
	{
		// 古いバッファの保存
		const auto& PrevShaderBufferList = m_ShaderBufferList;
		const auto& PrevTextureBindingLayoutList = m_TextureBindingLayoutList;

		// バッファの更新
		m_ShaderBufferList = ShaderBufferList;
		m_TextureBindingLayoutList = TextureBindingLayoutList;

		// 設定済みの値を反映
		// ShaderBuffer
		for (auto& ShaderBuffer : m_ShaderBufferList)
		{
			for (const auto& PrevShaderBuffer : PrevShaderBufferList)
			{
				if (ShaderBuffer->GetBufferName() != PrevShaderBuffer->GetBufferName())
				{
					const auto& Descriptor = ShaderBuffer->GetDescriptor();

					for (const auto& UniformDataMap : Descriptor->GetDataList())
					{
						const auto& UniformData = UniformDataMap.second;

						std::vector<unsigned char> PrevValue = PrevShaderBuffer->GetUniformValue(UniformData.UniformName);

						ShaderBuffer->SetValue(&PrevValue[0], UniformData.ByteOffset, UniformData.ByteSize);
					}
				}
			}
		}

		// TextureBuffer
		for (auto& TextureBindingLayout : m_TextureBindingLayoutList)
		{
			for (const auto& PrevTextureBindingLayout : PrevTextureBindingLayoutList)
			{
				if (TextureBindingLayout.TextureName == PrevTextureBindingLayout.TextureName)
				{
					TextureBindingLayout.TextureName = PrevTextureBindingLayout.TextureName;
					TextureBindingLayout.ViewBindingIndex = PrevTextureBindingLayout.ViewBindingIndex;
					TextureBindingLayout.SamplerBindingIndex = PrevTextureBindingLayout.SamplerBindingIndex;
					TextureBindingLayout.TextureIndex = PrevTextureBindingLayout.TextureIndex;
					TextureBindingLayout.TextureUsage = PrevTextureBindingLayout.TextureUsage;
				}
			}
		}

		return true;
	}

	bool CMaterial::CreateRefTextureList(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		m_RefTextureMap.clear();
		m_RefCubeMapMap.clear();
		m_RefFrameTextureMap.clear();
		m_RefDiffuse_Tex = nullptr;
		m_RefSpecular_Tex = nullptr;
		m_RefGGXLUT_Tex = nullptr;

		// 参照中のテクスチャリストを生成
		std::vector<std::shared_ptr<graphics::CTexture>> TextureList(0);
		if (TextureSet) TextureList = TextureSet->Get2DTextureList();

		std::vector<std::shared_ptr<graphics::CTexture>> CubeMapList(0);
		if (TextureSet) CubeMapList = TextureSet->GetCubeMapList();

		std::vector<std::shared_ptr<graphics::CTexture>> FrameTextureList(0);
		if (TextureSet) FrameTextureList = TextureSet->GetFrameTextureList();

		std::shared_ptr<graphics::CTexture> Diffuse_Tex = nullptr;
		if (TextureSet) Diffuse_Tex = TextureSet->GetDiffuse_Tex();

		std::shared_ptr<graphics::CTexture> Specular_Tex = nullptr;
		if (TextureSet) Specular_Tex = TextureSet->GetSpecular_Tex();

		std::shared_ptr<graphics::CTexture> GGXLUT_Tex = nullptr;
		if (TextureSet) GGXLUT_Tex = TextureSet->GetGGXLUT_Tex();

		size_t TexLayoutSize = m_TextureBindingLayoutList.size() * 2; // ImageViewとSamplerがあるので2倍にしている

		// テクスチャ
		for (int ImageInfoIndex = 0, TextureBindingLayoutIndex = 0; ImageInfoIndex < TexLayoutSize; ImageInfoIndex += 2, TextureBindingLayoutIndex++)
		{
			const auto& TexLayout = m_TextureBindingLayoutList[TextureBindingLayoutIndex];

			int TextureIndex = TexLayout.TextureIndex;

			if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && TextureIndex < TextureList.size()) ? TextureList[TextureIndex] : m_EmptyTexture;

				m_RefTextureMap.emplace(TexLayout.TextureName, Texture);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && TextureIndex < CubeMapList.size()) ? CubeMapList[TextureIndex] : m_EmptyCubeTexture;

				m_RefCubeMapMap.emplace(TexLayout.TextureName, Texture);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && TextureIndex < FrameTextureList.size()) ? FrameTextureList[TextureIndex] : m_EmptyTexture;

				m_RefFrameTextureMap.emplace(TexLayout.TextureName, Texture);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && Diffuse_Tex) ? Diffuse_Tex : m_EmptyTexture;

				m_RefDiffuse_Tex = Texture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && Specular_Tex) ? Specular_Tex : m_EmptyTexture;

				m_RefSpecular_Tex = Texture;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT)
			{
				const std::shared_ptr<graphics::CTexture>& Texture = (TextureIndex >= 0 && GGXLUT_Tex) ? GGXLUT_Tex : m_EmptyTexture;

				m_RefGGXLUT_Tex = Texture;
			}
		}

		return true;
	}

	bool CMaterial::ReCreateRefTextureList(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		// 参照テクスチャリストの生成
		// 減ることはあっても増えることはないのでTextureSetは不要

		// 古い情報を一時保存する
		const auto PrevRefTextureMap = m_RefTextureMap;
		const auto PrevRefCubeMapMap = m_RefCubeMapMap;
		const auto PrevRefFrameTextureMap = m_RefFrameTextureMap;
		const auto PrevRefDiffuse_Tex = m_RefDiffuse_Tex;
		const auto PrevRefSpecular_Tex = m_RefSpecular_Tex;
		const auto PrevRefGGXLUT_Tex = m_RefGGXLUT_Tex;

		// クリア
		m_RefTextureMap.clear();
		m_RefCubeMapMap.clear();
		m_RefFrameTextureMap.clear();
		m_RefDiffuse_Tex = nullptr;
		m_RefSpecular_Tex = nullptr;
		m_RefGGXLUT_Tex = nullptr;

		size_t TexLayoutSize = m_TextureBindingLayoutList.size() * 2; // ImageViewとSamplerがあるので2倍にしている

		// テクスチャ
		for (int ImageInfoIndex = 0, TextureBindingLayoutIndex = 0; ImageInfoIndex < TexLayoutSize; ImageInfoIndex += 2, TextureBindingLayoutIndex++)
		{
			const auto& TexLayout = m_TextureBindingLayoutList[TextureBindingLayoutIndex];

			if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_2D)
			{
				const auto& it = PrevRefTextureMap.find(TexLayout.TextureName);
				if (it != PrevRefTextureMap.end()) m_RefTextureMap.emplace(it->first, it->second);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_CUBE)
			{
				const auto& it = PrevRefCubeMapMap.find(TexLayout.TextureName);
				if (it != PrevRefCubeMapMap.end()) m_RefCubeMapMap.emplace(it->first, it->second);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_FRAME)
			{
				const auto& it = PrevRefFrameTextureMap.find(TexLayout.TextureName);
				if (it != PrevRefFrameTextureMap.end()) m_RefFrameTextureMap.emplace(it->first, it->second);
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Diffuse)
			{
				m_RefDiffuse_Tex = PrevRefDiffuse_Tex;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_Specular)
			{
				m_RefSpecular_Tex = PrevRefSpecular_Tex;
			}
			else if (TexLayout.TextureUsage == graphics::ETextureUsage::TEXTURE_USAGE_IBL_GGXLUT)
			{
				m_RefGGXLUT_Tex = PrevRefGGXLUT_Tex;
			}
		}

		return true;
	}

	std::shared_ptr<graphics::CMaterial> CMaterial::GetDepthMaterial() 
	{
		return m_DepthMaterial; 
	}

	void CMaterial::SetEnabledZWrite(bool Flag)
	{
		m_EnabledZWrite = Flag;
	}

	bool CMaterial::IsEnabledZWrite() const
	{
		return m_EnabledZWrite;
	}

	void CMaterial::SetDepthFunc(EDepthFunc DepthFunc)
	{
		m_DepthFunc = DepthFunc;
	}

	EDepthFunc CMaterial::GetDepthFunc() const
	{
		return m_DepthFunc;
	}

	void CMaterial::SetCullMode(ECullMode CullMode)
	{
		m_CullMode = CullMode;
	}

	void CMaterial::ResetToDefaultCullMode()
	{
		m_CullMode = m_DefaultCullMode;
	}

	ECullMode CMaterial::GetCullMode() const
	{
		return m_CullMode;
	}

	void CMaterial::SetBlendType(EBlendType BlendType)
	{
		m_BlendType = BlendType;
	}

	EBlendType CMaterial::GetBlendType() const
	{
		return m_BlendType;
	}

	void CMaterial::AddShaderBuffer(const std::shared_ptr<CShaderBuffer>& Buffer)
	{
		Buffer->RecalculateBindingLayoutOffset();

		Buffer->ResizePowerOfTwo(); // バッファサイズを2のn乗にする

		m_ShaderBufferList.push_back(std::make_shared<CShaderBuffer>(*Buffer));
	}
	
	void CMaterial::AddTextureBindingLayout(const STextureBindingLayout& Layout)
	{
		m_TextureBindingLayoutList.push_back(Layout);
	}

	void CMaterial::ReplaceTextureIndex(const std::string& TextureName, int TextureIndex)
	{
		for (auto& Layout : m_TextureBindingLayoutList)
		{
			if (TextureName != Layout.TextureName) continue;

			Layout.TextureIndex = TextureIndex;

			break;
		}
	}

	void CMaterial::ReplacePreloadUniformValue(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
	{
		for (const auto& Buffer : m_ShaderBufferList)
		{
			Buffer->ReplaceData(Name, Data, ByteSize, BindingIndex);
		}
	}

	void CMaterial::SetUniformValue(const std::string Name, const void* Data, int ByteSize, int DynamicOffsetNum)
	{
	}

	void CMaterial::AddRefCount()
	{
		m_RefCount++;
	}

	int CMaterial::GetRefCount() const
	{
		return m_RefCount;
	}

	bool CMaterial::IsUseDynamicOffset()
	{
		return (m_RefCount > 1);
	}

	void CMaterial::IncreaseDynamicOffset()
	{
		m_CurrentDynamicOffset++;
	}

	int CMaterial::GetDynamicOffset() const
	{
		return m_CurrentDynamicOffset;
	}

	void CMaterial::ResetDynamicOffset()
	{
		// ダイナミックオフセットは１から使用できるので初期値も１にする
		m_CurrentDynamicOffset = 1;
	}

	const std::vector<uint32_t>& CMaterial::GetBindingRefSizeList() const
	{
		return m_BindingRefSizeList;
	}

	void CMaterial::SetIsDrawOutline(bool Frag)
	{
		m_IsDrawOutline = Frag;
	}

	bool CMaterial::IsDrawOutline() const
	{
		return m_IsDrawOutline;
	}
}