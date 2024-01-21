#include "CMaterial.h"
#include "CMaterialFrame.h"

namespace graphics
{
	CMaterial::CMaterial(const std::shared_ptr<CMaterialCreateInfo>& createInfo):
		m_CreateInfo(createInfo),
		m_RefCount(0),
		m_DepthMaterial(nullptr),
		m_EnabledZTest(true),
		m_CullMode(ECullMode::CULL_BACK),
		m_BlendType(EBlendType::BLEND_TYPE_ADDITIVE)
	{
		ResetRefCount();
	}

	bool CMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		return true;
	}

	bool CMaterial::CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF)
	{
		m_DepthMaterial = DepthMF->CreateMaterial(pGraphicsAPI);

		m_DepthMaterial->SetRefCount(m_RefCount);

		m_DepthMaterial->SetCullMode(graphics::ECullMode::CULL_FRONT);

		if (!m_DepthMaterial->Create(nullptr)) return false;

		return true;
	}

	std::shared_ptr<graphics::CMaterial> CMaterial::GetDepthMaterial() 
	{
		return m_DepthMaterial; 
	}

	void CMaterial::SetEnabledZTest(bool EnabledZTest)
	{
		m_EnabledZTest = EnabledZTest;
	}

	bool CMaterial::IsEnabledZTest() const
	{
		return m_EnabledZTest;
	}

	void CMaterial::SetCullMode(ECullMode CullMode)
	{
		m_CullMode = CullMode;
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

	void CMaterial::IncreaseRefCount()
	{
		m_RefCount++;
	}

	int CMaterial::GetRefCount() const
	{
		return m_RefCount;
	}

	void CMaterial::SetRefCount(int RefCount)
	{
		m_RefCount = RefCount;
	}

	void CMaterial::ResetRefCount()
	{
		// ダイナミックオフセットは１から使用できるので初期値も１にする
		m_RefCount = 1;
	}

	const std::vector<uint32_t>& CMaterial::GetBindingRefSizeList() const
	{
		return m_BindingRefSizeList;
	}
}