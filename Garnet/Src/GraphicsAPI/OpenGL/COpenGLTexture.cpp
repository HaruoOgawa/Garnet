#include "COpenGLTexture.h"

#ifdef USE_OPENGL
namespace api
{
	COpenGLTexture::COpenGLTexture(api::COpenGLAPI* pGraphicsAPI, bool UseMipMap) :
		CTexture(UseMipMap),
		m_pGraphicsAPI(pGraphicsAPI)
	{
	}

	COpenGLTexture::~COpenGLTexture()
	{

	}

	bool COpenGLTexture::CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat)
	{
		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool COpenGLTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		return true;
	}

#endif
}
#endif // USE_OPENGL