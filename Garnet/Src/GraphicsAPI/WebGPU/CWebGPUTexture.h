#pragma once
#ifdef __DAWN__
#include "../../Graphics/CTexture.h"

namespace api
{
	class CWebGPUAPI;

	class CWebGPUTexture : public graphics::CTexture
	{
		// API
		api::CWebGPUAPI* m_pGraphicsAPI;
	public:
		CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI);
		virtual ~CWebGPUTexture();

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
#endif
	};
}
#endif