#include "CMaterial.h"

namespace graphics
{
	CMaterial::CMaterial():
		m_CreateInfo(nullptr),
		m_RefCount(0),
		m_UseDynamicUniform(false)
	{
	}

	void CMaterial::SetCreateInfo(const std::shared_ptr<CMaterialCreateInfo>& createInfo)
	{
		m_CreateInfo = createInfo;
	}

	bool CMaterial::Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList)
	{
		return true;
	}

	bool CMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	void CMaterial::AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer)
	{
		m_UniformBufferList.push_back(Buffer);
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

	const std::vector<uint32_t>& CMaterial::GetBindingRefSizeList() const
	{
		return m_BindingRefSizeList;
	}

	bool CMaterial::IsUseDynamicUniform() const
	{
		return m_UseDynamicUniform;
	}
}