#include "CMaterial.h"

namespace graphics
{
	CMaterial::CMaterial()
	{
	}

	bool CMaterial::Create(api::IGraphicsAPI* pGraphicsAPI, const CMaterialCreateInfo& createInfo)
	{
		return true;
	}

	bool CMaterial::Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection)
	{
		return true;
	}

	void CMaterial::AddUniformBufferDescriptor(const std::shared_ptr<CUniformBufferDescriptor>& Desc)
	{
		m_UniformBufferDescList.push_back(Desc);
	}
	
	void CMaterial::AddTextureBufferDescriptor(const std::shared_ptr<CTextureBufferDescriptor>& Desc)
	{
		m_TextureBufferDescList.push_back(Desc);
	}

	void CMaterial::SetUniformValue(const std::string Name, const void* Value)
	{
	}
}