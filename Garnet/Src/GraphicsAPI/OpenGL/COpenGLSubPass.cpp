#ifdef USE_OPENGL
#include "COpenGLSubPass.h"
#include "COpenGLTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Message/Console.h"

namespace api
{
	COpenGLSubPass::COpenGLSubPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat) :
		m_pGraphicsAPI(pGraphicsAPI),

		m_PassName(PassName),
		m_Width(0),
		m_Height(0),
		m_RenderPassFormat(RenderPassFormat),
		m_PassState(graphics::SRenderPassState()),
		m_DepthTexture(nullptr),
		m_FrameBuffer(-1),
		m_DepthBuffer(-1)
	{
	}

	COpenGLSubPass::~COpenGLSubPass()
	{
	}

	std::shared_ptr<graphics::CTexture> COpenGLSubPass::GetFrameTexture(int Index)
	{
		if (Index < 0 || Index >= static_cast<int>(m_FrameTextureList.size())) return nullptr;

		return m_FrameTextureList[Index];
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& COpenGLSubPass::GetFrameTextureList() const
	{
		return m_FrameTextureList;
	}

	const std::shared_ptr<graphics::CTexture>& COpenGLSubPass::GetDepthTexture() const
	{
		return m_DepthTexture;
	}

	bool COpenGLSubPass::Create(int Width, int Height, const graphics::SRenderPassState& PassState, bool IsMSAASubPass)
	{
		m_PassState = PassState;
		m_Width = Width;
		m_Height = Height;
		
		// フレームバッファの作成
		if (!CreateFrameBuffer()) return false;

		// カラーバッファの作成
		for (int AttachmentIndex = 0; AttachmentIndex < PassState.RenderTargetCount; AttachmentIndex++) { if (!CreateColorBuffer(AttachmentIndex, PassState, IsMSAASubPass)) return false; }

		// デプスバッファの作成
		if (!CreateDepthBuffer(PassState, IsMSAASubPass)) return false;

		// フレームバッファに使用するカラーバッファを指定
		std::vector<unsigned int> Attachments;
		for (int AttachmentIndex = 0; AttachmentIndex < PassState.RenderTargetCount; AttachmentIndex++) { Attachments.push_back(GL_COLOR_ATTACHMENT0 + AttachmentIndex); }

		//
		int BufferCount = PassState.RenderTargetCount;

		// カラーバッファのアウトプットが複数個ある時にデプスの方もアタッチするようにすると1つしかカラーがアウトプットされなくなるので深度テクスチャが不要なら追加しない
		if (PassState.DepthBuffer && PassState.DepthTexture)
		{
			// Depth_Stencilの分を追加しておく
			BufferCount += 1;

			if (m_PassState.Stencil)
			{
				Attachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
			}
			else
			{
				Attachments.push_back(GL_DEPTH_ATTACHMENT);
			}
		}

		glDrawBuffers(BufferCount, &Attachments[0]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // 後続の描画が映らなくなるのでバインドを解除しておく

		m_Attachments = Attachments;

		return true;
	}

	bool COpenGLSubPass::CreateFrameBuffer()
	{
		glGenFramebuffers(1, &m_FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

		return true;
	}

	bool COpenGLSubPass::CreateColorBuffer(int AttachmentIndex, const graphics::SRenderPassState& PassState, bool IsMSAASubPass)
	{
		// カラーバッファを作成しない
		if (!PassState.ColorBuffer) return true;

		if (PassState.ColorTexture)
		{
			auto FrameTexture = m_pGraphicsAPI->CreateTexture(false);
			if (!FrameTexture->CreateFrameTexture(m_Width, m_Height, m_RenderPassFormat, 1, true)) return false;
			m_FrameTextureList.push_back(FrameTexture);

			COpenGLTexture* pOpenGLTexture = static_cast<COpenGLTexture*>(FrameTexture.get());

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + AttachmentIndex, GL_TEXTURE_2D, pOpenGLTexture->GetTextureID(), 0);
		}
		else
		{
			GLuint ColorBuffer;

			glGenRenderbuffers(1, &ColorBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, ColorBuffer);

			GLenum internalformat = GL_RGBA8;
			switch (m_RenderPassFormat)
			{
			case api::ERenderPassFormat::NONE:
				break;
			case api::ERenderPassFormat::COLOR_RENDERPASS:
				internalformat = GL_RGBA8;
				break;
			case api::ERenderPassFormat::COLOR_FLOAT_RENDERPASS:
				internalformat = GL_RGBA16F;
				break;
			default:
				break;
			}

			GLenum attachment = GL_COLOR_ATTACHMENT0 + AttachmentIndex;

			// アンチエイリアス
			if (IsMSAASubPass)
			{
				int maxSamples = 0;
				glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

				int sampleCount = std::min(std::max(PassState.AASampleNum, 1), maxSamples);

				glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, internalformat, m_Width, m_Height);
			}
			else
			{
				glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_Width, m_Height);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, ColorBuffer);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			m_ColorBufferList.push_back(ColorBuffer);
		}

		return true;
	}

	bool COpenGLSubPass::CreateDepthBuffer(const graphics::SRenderPassState& PassState, bool IsMSAASubPass)
	{
		// デプスバッファを生成しない
		if (!PassState.DepthBuffer) return true;

		if (PassState.DepthTexture)
		{
			m_DepthTexture = m_pGraphicsAPI->CreateTexture(false);
			if (!m_DepthTexture->CreateFrameTexture(m_Width, m_Height, api::ERenderPassFormat::DEPTH_FLOAT_RENDERPASS, 1, true)) return false;

			COpenGLTexture* pOpenGLTexture = static_cast<COpenGLTexture*>(m_DepthTexture.get());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pOpenGLTexture->GetTextureID(), 0);
		}
		else
		{
			glGenRenderbuffers(1, &m_DepthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);

			GLenum internalformat = GL_DEPTH_COMPONENT32F;
			GLenum attachment = GL_DEPTH_ATTACHMENT;
			if (PassState.Stencil)
			{
				internalformat = GL_DEPTH24_STENCIL8;
				attachment = GL_DEPTH_STENCIL_ATTACHMENT;
			}

			// アンチエイリアス
			if (IsMSAASubPass)
			{
				int maxSamples = 0;
				glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

				int sampleCount = std::min(std::max(PassState.AASampleNum, 1), maxSamples);

				glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, internalformat, m_Width, m_Height);
			}
			else
			{
				glRenderbufferStorage(GL_RENDERBUFFER, internalformat, m_Width, m_Height);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_DepthBuffer);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		return true;
	}

	bool COpenGLSubPass::BeginRenderPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
		glViewport(0, 0, m_Width, m_Height);

		GLbitfield clearMask = 0;

		if (m_PassState.ClearColor) clearMask |= GL_COLOR_BUFFER_BIT;

		if (m_PassState.ClearDepth)
		{
			glClearDepth(1.0f);
			clearMask |= GL_DEPTH_BUFFER_BIT;
		}

		if (m_PassState.Stencil && m_PassState.ClearStencil)
		{
			glClearStencil(0);

			clearMask |= GL_STENCIL_BUFFER_BIT;

			// glStencilMaskはglColorMask・glDepthMaskと同じ関数でフレームバッファへの書き込みを有効にしたり無効にしたりする
			// 0xFFにすることで有効になる?
			glStencilMask(0xff);
		}

		if (clearMask != 0)
		{
			glClear(clearMask);
		}

		// カラーバッファ単位で初期化
		if (m_PassState.ClearColor)
		{
			if (m_PassState.RenderTargetCount != static_cast<int>(m_PassState.InitColorList.size())) return false;

			for (int i = 0; i < m_PassState.RenderTargetCount; i++)
			{
				const auto& InitColor = m_PassState.InitColorList[i];

				GLfloat clearColor[4] = { InitColor.r, InitColor.g, InitColor.b, InitColor.a };

				glClearBufferfv(GL_COLOR, i, clearColor);
			}
		}

		return true;
	}

	bool COpenGLSubPass::EndRenderPass()
	{
		return true;
	}

	int COpenGLSubPass::GetWidth() const
	{
		return m_Width;
	}

	int COpenGLSubPass::GetHeight() const
	{
		return m_Height;
	}

	GLuint COpenGLSubPass::GetFrameBuffer() const
	{
		return m_FrameBuffer;
	}

	// Attachment
	const std::vector<unsigned int>& COpenGLSubPass::GetAttachments() const
	{
		return m_Attachments;
	}
}
#endif