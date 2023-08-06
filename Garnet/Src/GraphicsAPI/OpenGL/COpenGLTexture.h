#pragma once

#ifdef USE_OPENGL
#include "../../Graphics/CTexture.h"
#include "COpenGLAPI.h"

namespace api
{
	class COpenGLTexture : public graphics::CTexture
	{
		// API
		api::COpenGLAPI* m_pGraphicsAPI;

		//
		GLuint m_TextureID;
	public:
		COpenGLTexture(api::COpenGLAPI* pGraphicsAPI, bool UseMipMap);
		virtual ~COpenGLTexture();

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat) override;
#ifdef USE_TEXTURE_LOADER
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
#endif
		void SetActive(GLenum texture);
		void SetEactive(GLenum texture);

		GLuint GetTextureID() const;
	};
}
#endif // USE_OPENGL