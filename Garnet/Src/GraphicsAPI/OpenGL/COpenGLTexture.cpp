#include "COpenGLTexture.h"

#ifdef USE_OPENGL
namespace api
{
	COpenGLTexture::COpenGLTexture(api::COpenGLAPI* pGraphicsAPI, bool UseMipMap, const graphics::STextureSamplerParam& SamplerParam) :
		CTexture(UseMipMap, SamplerParam),
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
			break;
		}
		
		case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
		{
			internalformat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
		}
		
		case api::ERenderPassFormat::DEPTH_RENDERPASS:
		{
			internalformat = GL_DEPTH_COMPONENT;
			format = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_BYTE;
			break;

		}
		case api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS:
		{
			internalformat = GL_DEPTH_COMPONENT;
			format = GL_DEPTH_COMPONENT;
			type = GL_FLOAT;
			break;
		}
		
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
		CreateTextureSampler(GL_TEXTURE_2D);
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, type, 0);

		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

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

		if (m_TextureType == graphics::ETextureType::TEXTURE_2D)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			
			CreateTextureSampler(GL_TEXTURE_2D);
			
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, type, &pixelData[0]);

			glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			if(m_UseMipMap) glGenerateMipmap(GL_TEXTURE_2D); // ミップマップを生成

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
			
			CreateTextureSampler(GL_TEXTURE_CUBE_MAP);
			
			for (unsigned int i = 0; i < 6; i++)
			{
				size_t byteSize = m_Width * m_Height * 4;
				size_t byteOffsset = i * byteSize;

				std::vector<unsigned char> data;
				data.resize(byteSize);
				std::memcpy(&data[0], &pixelData[byteOffsset], byteSize);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, m_Width, m_Height, 0, format, type, &data[0]);
			}
			
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // ミップマップをシームレスにする
			if (m_UseMipMap) glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // ミップマップを生成
			
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		return true;
	}

	void COpenGLTexture::CreateTextureSampler(GLenum target)
	{
		switch (m_SamplerParam.FilterMode)
		{
			case graphics::ETextureFilterMode::LINEAR:
			{
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (m_UseMipMap)? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				break;
			}
			case graphics::ETextureFilterMode::NEAREST:
			{
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				break;
			}
			default:
			{
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, (m_UseMipMap) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				break;
			}
		}

		switch (m_SamplerParam.WrapMode)
		{
			case graphics::ETextureWrapMode::CLAMP_TO_EDGE:
			{
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			}
			case graphics::ETextureWrapMode::REPEAT:
			{
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			}
			default:
			{
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			}
		}
	}

	void COpenGLTexture::SetActive(GLenum texture)
	{
		if (m_TextureType == graphics::ETextureType::TEXTURE_2D)
		{
			glActiveTexture(texture);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}
		else if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
		{
			glActiveTexture(texture);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
		}
	}

	void COpenGLTexture::SetEactive(GLenum texture)
	{
		if (m_TextureType == graphics::ETextureType::TEXTURE_2D)
		{
			glActiveTexture(texture);
			glDisable(GL_TEXTURE_2D);
		}
		else if (m_TextureType == graphics::ETextureType::TEXTURE_CUBE)
		{
			glActiveTexture(texture);
			glDisable(GL_TEXTURE_CUBE_MAP);
		}
	}

	GLuint COpenGLTexture::GetTextureID() const
	{
		return m_TextureID;
	}
}
#endif // USE_OPENGL