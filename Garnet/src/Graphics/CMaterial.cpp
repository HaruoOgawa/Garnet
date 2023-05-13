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
}