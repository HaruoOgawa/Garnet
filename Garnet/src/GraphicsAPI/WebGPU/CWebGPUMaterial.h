#pragma once

#include "../../Graphics/CMaterial.h"

namespace graphics{ class CMaterialCreateInfo; }

namespace api
{
	class CWebGPUMaterial : public graphics::CMaterial
	{
	public:
		CWebGPUMaterial();
		virtual ~CWebGPUMaterial();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const graphics::CMaterialCreateInfo& createInfo) override;
		bool Update(float SecondsTime) override;
	};
}