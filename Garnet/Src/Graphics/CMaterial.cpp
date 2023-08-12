#include "CMaterial.h"

namespace graphics
{
	CMaterial::CMaterial(const std::shared_ptr<CMaterialCreateInfo>& createInfo):
		m_CreateInfo(createInfo),
		m_RefCount(0),
		m_UseDynamicUniform(false),
		m_DepthMaterial(nullptr),
		m_EnabledZTest(true),
		m_CullMode(ECullMode::CULL_BACK)
	{
	}

	bool CMaterial::Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList)
	{
		return true;
	}

	bool CMaterial::CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<file::CFile>& DepthVertex, const std::shared_ptr<file::CFile>& DepthFragment)
	{
		std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
		createInfo->SetVertexShaderCode(DepthVertex->GetData());
		createInfo->SetFragmentShaderCode(DepthFragment->GetData());

		auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0) });
		UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
		UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

		m_DepthMaterial = pGraphicsAPI->CreateMaterial(createInfo);

		m_DepthMaterial->AddUniformBuffer(UniformBuffer);

		m_DepthMaterial->SetRefStatus(m_RefCount, m_UseDynamicUniform);

		m_DepthMaterial->SetCullMode(graphics::ECullMode::CULL_FRONT);

		if (!m_DepthMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;

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

	bool CMaterial::SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	void CMaterial::AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer)
	{
		Buffer->RecalculateBindingLayoutOffset();

		m_UniformBufferList.push_back(std::make_shared<CUniformBuffer>(*Buffer));
	}
	
	void CMaterial::AddTextureBindingLayout(const STextureBindingLayout& Layout)
	{
		m_TextureBindingLayoutList.push_back(Layout);
	}

	void CMaterial::SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum)
	{
	}

	void CMaterial::IncreaseRefCount()
	{
		m_RefCount++;

		if (m_RefCount > 1)
		{
			m_UseDynamicUniform = true;
		}
	}

	int CMaterial::GetRefCount() const
	{
		return m_RefCount;
	}

	void CMaterial::SetRefStatus(int RefCount, bool UseDynamicUniform)
	{
		m_RefCount = RefCount;
		m_UseDynamicUniform = UseDynamicUniform;
	}

	const std::vector<uint32_t>& CMaterial::GetBindingRefSizeList() const
	{
		return m_BindingRefSizeList;
	}

	bool CMaterial::IsUseDynamicUniform() const
	{
		return m_UseDynamicUniform;
	}
}