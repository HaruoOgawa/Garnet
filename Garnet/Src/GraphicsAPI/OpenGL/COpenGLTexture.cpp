#include "COpenGLTexture.h"

#ifdef USE_OPENGL
namespace api
{
	COpenGLTexture::COpenGLTexture(api::COpenGLAPI* pGraphicsAPI, bool UseMipMap) :
		CTexture(UseMipMap),
		m_pGraphicsAPI(pGraphicsAPI),
		m_TextureID(-1)
	{
	}

	COpenGLTexture::~COpenGLTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	bool COpenGLTexture::CreateFrameTexture(int Width, int Height, api::ERenderPassFormat RenderPassFormat)
	{
		m_Width = Width;
		m_Height = Height;
		m_RenderPassFormat = RenderPassFormat;

		GLuint internalformat;
		GLenum format;
		GLenum type = GL_UNSIGNED_BYTE;

		switch (m_RenderPassFormat)
		{
		case api::ERenderPassFormat::COLOR_RENDERPASS:
		{
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		break;
		case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
		{
			internalformat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_FLOAT;
		}
		break;
		case api::ERenderPassFormat::DEPTH_RENDERPASS:
		case api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS:
		{
			internalformat = GL_DEPTH24_STENCIL8;
			format = GL_DEPTH_STENCIL;
			type = GL_DEPTH24_STENCIL8;
		}
		break;
		default:
		{
			internalformat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		break;
		}

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, type, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

#ifdef USE_TEXTURE_LOADER
	bool COpenGLTexture::Create(const std::vector<unsigned char>& pixelData, int pixelSize)
	{
		GLuint internalformat;
		GLenum format;
		GLenum type = GL_UNSIGNED_BYTE;

		switch (m_RenderPassFormat)
		{
		case api::ERenderPassFormat::COLOR_RENDERPASS:
			{
				internalformat = GL_RGBA;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
			}
			break;
		case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
			{
				internalformat = GL_RGBA16F;
				format = GL_RGBA;
				type = GL_FLOAT;
			}
			break;
		case api::ERenderPassFormat::DEPTH_RENDERPASS:
		case api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS:
			{
				internalformat = GL_DEPTH24_STENCIL8;
				format = GL_DEPTH_STENCIL;
				type = GL_DEPTH24_STENCIL8;
			}
			break;
		default:
			{
				internalformat = GL_RGBA;
				format = GL_RGBA;
				type = GL_UNSIGNED_BYTE;
			}
			break;
		}

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, type, &pixelData[0]);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

#endif
	void COpenGLTexture::SetActive(GLenum texture)
	{
		glActiveTexture(texture);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void COpenGLTexture::SetEactive(GLenum texture)
	{
		glActiveTexture(texture);
		glDisable(GL_TEXTURE_2D);
	}

	GLuint COpenGLTexture::GetTextureID() const
	{
		return m_TextureID;
	}
}
#endif // USE_OPENGL