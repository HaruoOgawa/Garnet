#ifdef USE_OPENGL
#include "COpenGLRenderPass.h"
#include "COpenGLTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Message/Console.h"

namespace api
{
	COpenGLRenderPass::COpenGLRenderPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor) :
		m_pGraphicsAPI(pGraphicsAPI),

		m_PassName(PassName),
		m_Width(0),
		m_Height(0),
		m_InitColor(InitColor),
		m_RenderPassFormat(RenderPassFormat),
		m_DepthTexture(nullptr),

		m_FrameBuffer(-1),
		m_ColorBuffer(-1),
		m_DepthBuffer(-1)
	{
	}
	
	COpenGLRenderPass::~COpenGLRenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> COpenGLRenderPass::GetFrameTexture(int Index)
	{
		if (Index < 0 || Index >= static_cast<int>(m_FrameTextureList.size())) return nullptr;

		return m_FrameTextureList[Index];
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& COpenGLRenderPass::GetFrameTextureList() const
	{
		return m_FrameTextureList;
	}

	const std::shared_ptr<graphics::CTexture>& COpenGLRenderPass::GetDepthTexture() const
	{
		return m_DepthTexture;
	}

	bool COpenGLRenderPass::Create(int Width, int Height, int RenderTargetCount)
	{
		m_Width = Width;
		m_Height = Height;

		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++)
		{
			auto FrameTexture = m_pGraphicsAPI->CreateTexture(false);
			if (!FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;

			m_FrameTextureList.push_back(FrameTexture);
		}

		m_DepthTexture = m_pGraphicsAPI->CreateTexture(false);
		if (!m_DepthTexture->CreateFrameTexture(Width, Height, api::ERenderPassFormat::DEPTH_RENDERPASS)) return false;

		if (!CreateFrameBuffer()) return false; // フレームバッファの作成
		for(int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++){ if (!CreateColorBuffer(AttachmentIndex)) return false; } // カラーバッファの作成
		if (!CreateDepthBuffer()) return false; // デプスバッファの作成

		// フレームバッファに使用するカラーバッファを指定
		std::vector<unsigned int> Attachments;
		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++) { Attachments.push_back(GL_COLOR_ATTACHMENT0 + AttachmentIndex); }
		glDrawBuffers(RenderTargetCount, &Attachments[0]);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // 後続の描画が映らなくなるのでバインドを解除しておく

		return true;
	}

	bool COpenGLRenderPass::CreateFrameBuffer()
	{
		glGenFramebuffers(1, &m_FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

		return true;
	}

	bool COpenGLRenderPass::CreateColorBuffer(int AttachmentIndex)
	{
		if (AttachmentIndex < 0 || AttachmentIndex >= static_cast<int>(m_FrameTextureList.size())) return false;

		COpenGLTexture* pOpenGLTexture = static_cast<COpenGLTexture*>(m_FrameTextureList[AttachmentIndex].get());

		// ひとまずテクスチャだけ対応しておく
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + AttachmentIndex, GL_TEXTURE_2D, pOpenGLTexture->GetTextureID(), 0);

		return true;
	}

	bool COpenGLRenderPass::CreateDepthBuffer()
	{
		glGenRenderbuffers(1, &m_DepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBuffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_Width, m_Height);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBuffer);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return true;
	}

	bool COpenGLRenderPass::BeginRenderPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
		glViewport(0, 0, m_Width, m_Height);
		glClearColor(m_InitColor.r, m_InitColor.g, m_InitColor.b, m_InitColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		return true;
	}

	bool COpenGLRenderPass::EndRenderPass()
	{
		return true;
	}

	GLuint COpenGLRenderPass::GetFrameBuffer() const
	{
		return m_FrameBuffer;
	}
}
#endif