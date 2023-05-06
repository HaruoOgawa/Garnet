#include "CWebGPUMaterial.h"
#include "../CMaterialCreateInfo.h"

namespace api
{
	CWebGPUMaterial::CWebGPUMaterial():
		CMaterial()
	{
	}

	CWebGPUMaterial::~CWebGPUMaterial()
	{
	}

	bool CWebGPUMaterial::Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo)
	{
		return true;
	}

	bool CWebGPUMaterial::Update(float SecondsTime)
	{
		return true;
	}
}