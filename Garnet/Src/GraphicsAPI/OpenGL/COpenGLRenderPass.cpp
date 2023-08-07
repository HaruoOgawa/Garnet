#ifdef USE_OPENGL
#include "COpenGLRenderPass.h"
#include "COpenGLTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Debug/Message/Console.h"

namespace api
{
	COpenGLRenderPass::COpenGLRenderPass(api::COpenGLAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor) :
		m_pGraphicsAPI(pGraphicsAPI),

		m_PassName(PassName),
		m_Width(0),
		m_Height(0),
		m_InitColor(InitColor),
		m_RenderPassFormat(RenderPassFormat),
		m_FrameTexture(nullptr),
		m_DepthTexture(nullptr),

		m_FrameBuffer(-1),
		m_ColorBuffer(-1),
		m_DepthBuffer(-1)
	{
	}
	
	COpenGLRenderPass::~COpenGLRenderPass()
	{
	}

	std::shared_ptr<graphics::CTexture> COpenGLRenderPass::GetFrameTexture()
	{
		return m_FrameTexture;
	}

	std::shared_ptr<graphics::CTexture> COpenGLRenderPass::GetDepthTexture()
	{
		return m_DepthTexture;
	}

	bool COpenGLRenderPass::Create(int Width, int Height)
	{
		m_Width = Width;
		m_Height = Height;

		m_FrameTexture = std::make_shared<COpenGLTexture>(m_pGraphicsAPI, false);
		if (!m_FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;

		m_DepthTexture = std::make_shared<COpenGLTexture>(m_pGraphicsAPI, false);
		if (!m_DepthTexture->CreateFrameTexture(Width, Height, api::ERenderPassFormat::DEPTH_RENDERPASS)) return false;

		if (!CreateFrameBuffer()) return false; // フレームバッファの作成
		if (!CreateColorBuffer()) return false; // カラーバッファの作成
		if (!CreateDepthBuffer()) return false; // デプスバッファの作成

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // 後続の描画が映らなくなるのでバインドを解除しておく

		return true;
	}

	bool COpenGLRenderPass::CreateFrameBuffer()
	{
		glGenFramebuffers(1, &m_FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

		return true;
	}

	bool COpenGLRenderPass::CreateColorBuffer()
	{
		// ひとまずテクスチャだけ対応しておく
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameTexture->GetTextureID(), 0);

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
}
#endif