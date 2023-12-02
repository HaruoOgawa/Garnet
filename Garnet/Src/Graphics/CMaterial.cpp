#include "CMaterial.h"

namespace graphics
{
	CMaterial::CMaterial(const std::shared_ptr<CMaterialCreateInfo>& createInfo):
		m_CreateInfo(createInfo),
		m_RefCount(0),
		m_UseDynamicBufferOffset(false),
		m_DepthMaterial(nullptr),
		m_EnabledZTest(true),
		m_CullMode(ECullMode::CULL_BACK),
		m_BlendType(EBlendType::BLEND_TYPE_ADDITIVE)
	{
	}

	bool CMaterial::Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		return true;
	}

	bool CMaterial::CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<resource::CFile>& DepthVertex, const std::shared_ptr<resource::CFile>& DepthFragment)
	{
		std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
		createInfo->SetVertexShaderCode(DepthVertex->GetData());
		createInfo->SetFragmentShaderCode(DepthFragment->GetData());

		auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });
		UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

		m_DepthMaterial = pGraphicsAPI->CreateMaterial(createInfo);

		m_DepthMaterial->AddShaderBuffer(UniformBuffer);

		m_DepthMaterial->SetRefStatus(m_RefCount, m_UseDynamicBufferOffset);

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

	void CMaterial::ReplacePreloadUniformValue(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
	{
		for (const auto& Buffer : m_ShaderBufferList)
		{
			Buffer->ReplaceData(Name, Data, ByteSize, BindingIndex);
		}
	}

	void CMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
	}

	void CMaterial::IncreaseRefCount()
	{
		m_RefCount++;

		if (m_RefCount > 1)
		{
			m_UseDynamicBufferOffset = true;
		}
	}

	int CMaterial::GetRefCount() const
	{
		return m_RefCount;
	}

	void CMaterial::SetRefStatus(int RefCount, bool UseDynamicBufferOffset)
	{
		m_RefCount = RefCount;
		m_UseDynamicBufferOffset = UseDynamicBufferOffset;
	}

	const std::vector<uint32_t>& CMaterial::GetBindingRefSizeList() const
	{
		return m_BindingRefSizeList;
	}

	bool CMaterial::IsUseDynamicBufferOffset() const
	{
		return m_UseDynamicBufferOffset;
	}
}