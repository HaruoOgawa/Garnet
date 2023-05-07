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

	bool CMaterial::Update(float SecondsTime)
	{
		return true;
	}

	void CMaterial::AddBindingDescriptor(const std::shared_ptr<IDescriptor>& Desc)
	{
		m_BindingDescList.push_back(Desc);
	}
}