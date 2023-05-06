#include "CMaterial.h"
#include "CUniformBufferDescriptor.h"
#include "../Interface/IGraphicsAPI.h"

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
}