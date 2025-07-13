#ifdef USE_OPENGL
#include "COpenGLRenderPass.h"
#include "COpenGLSubPass.h"
#include "COpenGLTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Message/Console.h"

namespace api
{
	COpenGLRenderPass::COpenGLRenderPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat) :
		m_pGraphicsAPI(pGraphicsAPI),
		m_PassName(PassName),
		m_RenderPassFormat(RenderPassFormat),
		m_UseColorBuffer(false),
		m_UseDepthBuffer(false),
		m_UseMSAA(false),
		m_SubPass(nullptr),
		m_ResolveSubPass(nullptr)
	{
	}
	
	COpenGLRenderPass::~COpenGLRenderPass()
	{
	}

	bool COpenGLRenderPass::Create(int Width, int Height, const graphics::SRenderPassState& PassState)
	{
		m_UseColorBuffer = PassState.ColorBuffer;
		m_UseDepthBuffer = PassState.DepthBuffer;
		m_UseMSAA = PassState.EnabledAA;

		if (m_UseMSAA)
		{
			{
				graphics::SRenderPassState SubPassState = graphics::SRenderPassState(PassState.RenderTargetCount);
				SubPassState.ColorBuffer = true;
				SubPassState.ColorTexture = false;
				SubPassState.DepthBuffer = true;

				// 引き継ぐ
				SubPassState.RenderTargetCount = PassState.RenderTargetCount;
				SubPassState.EnabledAA = PassState.EnabledAA;
				SubPassState.AASampleNum = PassState.AASampleNum;
				SubPassState.Stencil = PassState.Stencil; 
				SubPassState.InitColorList = PassState.InitColorList;

				m_SubPass = std::make_shared<COpenGLSubPass>(m_pGraphicsAPI, m_PassName, m_RenderPassFormat);
				if (!m_SubPass->Create(Width, Height, SubPassState, true)) return false;
			}

			{
				graphics::SRenderPassState SubPassState = graphics::SRenderPassState(PassState.RenderTargetCount);
				SubPassState.ColorBuffer = true;
				SubPassState.ColorTexture = true;
				SubPassState.DepthBuffer = true;

				// 引き継ぐ
				SubPassState.RenderTargetCount = PassState.RenderTargetCount;
				SubPassState.Stencil = PassState.Stencil; 
				SubPassState.InitColorList = PassState.InitColorList;

				m_ResolveSubPass = std::make_shared<COpenGLSubPass>(m_pGraphicsAPI, m_PassName, m_RenderPassFormat);
				if (!m_ResolveSubPass->Create(Width, Height, SubPassState, false)) return false;
			}
		}
		else
		{
			graphics::SRenderPassState SubPassState = PassState;

			m_SubPass = std::make_shared<COpenGLSubPass>(m_pGraphicsAPI, m_PassName, m_RenderPassFormat);
			if (!m_SubPass->Create(Width, Height, SubPassState, false)) return false;
		}

		return true;
	}

	bool COpenGLRenderPass::BeginRenderPass()
	{
		if (!m_SubPass->BeginRenderPass()) return false;

		return true;
	}

	bool COpenGLRenderPass::EndRenderPass()
	{
		if (!m_SubPass->EndRenderPass()) return false;

		// MSAAが有効ならフレームバッファのコピーを行う
		if (m_UseMSAA)
		{
			if (!CopyRenderPass()) return false;
		}

		return true;
	}

	bool COpenGLRenderPass::CopyRenderPass()
	{
		GLuint SrcFrameBuffer = m_SubPass->GetFrameBuffer();
		GLuint DstFrameBuffer = m_ResolveSubPass->GetFrameBuffer();

		if (SrcFrameBuffer == -1 || DstFrameBuffer == -1) return false;

		//
		int Width = m_SubPass->GetWidth();
		int Height = m_SubPass->GetHeight();

		// カラーデプスアタッチメントの数だけバッファのコピーを行う
		for (const auto& Attachment : m_ResolveSubPass->GetAttachments())
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, SrcFrameBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DstFrameBuffer);

			glReadBuffer(Attachment);
			glDrawBuffer(Attachment);

			GLbitfield mask = 0;
			if (Attachment == GL_DEPTH_ATTACHMENT || Attachment == GL_DEPTH_STENCIL_ATTACHMENT)
			{
				// デプスアタッチメント
				mask |= GL_DEPTH_BUFFER_BIT;
			}
			else
			{
				// カラーアタッチメント
				mask |= GL_COLOR_BUFFER_BIT;
			}

			glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, mask, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		return true;
	}

	std::shared_ptr<graphics::CTexture> COpenGLRenderPass::GetFrameTexture(int Index)
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetFrameTexture(Index);
		}
		else
		{
			return m_SubPass->GetFrameTexture(Index);
		}
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& COpenGLRenderPass::GetFrameTextureList() const
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetFrameTextureList();
		}
		else
		{
			return m_SubPass->GetFrameTextureList();
		}
	}

	const std::shared_ptr<graphics::CTexture>& COpenGLRenderPass::GetDepthTexture() const
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetDepthTexture();
		}
		else
		{
			return m_SubPass->GetDepthTexture();
		}
	}

	int COpenGLRenderPass::GetWidth() const
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetWidth();
		}
		else
		{
			return m_SubPass->GetWidth();
		}
	}

	int COpenGLRenderPass::GetHeight() const
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetHeight();
		}
		else
		{
			return m_SubPass->GetHeight();
		}
	}

	GLuint COpenGLRenderPass::GetFrameBuffer() const
	{
		if (m_UseMSAA)
		{
			return m_ResolveSubPass->GetFrameBuffer();
		}
		else
		{
			return m_SubPass->GetFrameBuffer();
		}
	}
}
#endif