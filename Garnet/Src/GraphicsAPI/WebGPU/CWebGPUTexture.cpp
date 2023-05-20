#ifdef __DAWN__
#include "CWebGPUTexture.h"
#include "CWebGPUAPI.h"
namespace api
{
	CWebGPUTexture::CWebGPUTexture(api::CWebGPUAPI* pGraphicsAPI):
		CTexture(),
		m_pGraphicsAPI(pGraphicsAPI)
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