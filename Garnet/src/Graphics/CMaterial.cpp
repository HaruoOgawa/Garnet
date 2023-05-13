#include "CMaterial.h"

namespace graphics
{
	CMaterial::CMaterial():
		m_CreateInfo(nullptr),
		m_RefCount(0)
	{
	}

	void CMaterial::SetCreateInfo(const std::shared_ptr<CMaterialCreateInfo>& createInfo)
	{
		m_CreateInfo = createInfo;
	}

	bool CMaterial::Create(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		return true;
	}

	void CMaterial::AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer)
	{
		m_UniformBufferList.push_back(Buffer);
	}
	
	void CMaterial::AddTextureBuffer(const std::shared_ptr<CTextureBuffer>& Buffer)
	{
		m_TextureBufferList.push_back(Buffer);
	}

	void CMaterial::SetUniformValue(const std::string Name, const void* Value)
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
}