#pragma once
#ifdef __DAWN__
#include "../../Graphics/CTexture.h"

namespace api
{
	class CWebGPUTexture : public graphics::CTexture
	{
	public:
		CWebGPUTexture();
		virtual ~CWebGPUTexture();

#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
#endif
	};
}
#endif