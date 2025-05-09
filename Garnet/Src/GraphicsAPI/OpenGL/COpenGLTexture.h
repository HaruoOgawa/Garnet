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

	private:
		void CreateTextureSampler(GLenum target);

	public:
		COpenGLTexture(api::COpenGLAPI* pGraphicsAPI, bool UseMipMap, const graphics::STextureSamplerParam& SamplerParam);
		virtual ~COpenGLTexture();

		virtual bool CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat, int AASampleNum, bool ReadOnShader) override;
		
		virtual bool Create(const std::vector<unsigned char>& pixelData, int pixelSize) override;
		
		void SetActive(GLenum texture);
		void SetEactive(GLenum texture);

		GLuint GetTextureID() const;
	};
}
#endif // USE_OPENGL