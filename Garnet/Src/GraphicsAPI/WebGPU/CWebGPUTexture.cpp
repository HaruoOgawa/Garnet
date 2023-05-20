#include "CWebGPUTexture.h"
#ifdef __DAWN__
namespace api
{
	CWebGPUTexture::CWebGPUTexture():
		CTexture()
	{
	}

	CWebGPUTexture::~CWebGPUTexture()
	{
	}

#ifdef USE_TEXTURE_LOADER
	bool CWebGPUTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		return true;
	}
#endif
}
#endif