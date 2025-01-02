#ifdef USE_OPENGL

#include "COpenGLAPI.h"

#define GL_IMPLEMENTATION
#include "glDef.h"

#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"
#include "COpenGLVertexBuffer.h"
#include "COpenGLIndexBuffer.h"
#include "COpenGLRenderer.h"
#include "COpenGLRenderPass.h"
#include "COpenGLGPGPUHandler.h"

namespace api
{
	COpenGLAPI::COpenGLAPI(int Width, int Height):
		CGraphicsAPI(),
		m_VertexShaderExtension(".vert"),
		m_FragmentShaderExtension(".frag"),
		m_GeometryShaderExtension(".geom"),
		m_HullShaderExtension(".tesc"),
		m_DomainShaderExtension(".tese"),
		m_ComputeShaderExtension(".comp"),
		m_Width(Width),
		m_Height(Height)
	{
	}

	COpenGLAPI::~COpenGLAPI()
	{
	}

	bool COpenGLAPI::Initialize(window::IWindowAPI* pWindowAPI)
	{
		if (!InitGL()) return false;

		return true;
	}

	void COpenGLAPI::Release()
	{
	}

	bool COpenGLAPI::CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width, int Height, int RenderTargetCount,
		bool UseColorTexture, bool UseDepthTexture, bool UseStencil)
	{
		std::shared_ptr<COpenGLRenderPass> RenderPass = std::make_shared<COpenGLRenderPass>(this, PassName, RenderPassFormat, InitColor);

		if (Width != -1 && Height != -1)
		{
			if (!RenderPass->Create(Width, Height, RenderTargetCount, UseColorTexture, UseDepthTexture, UseStencil)) return false;
		}
		else
		{
			if (!RenderPass->Create(m_Width, m_Height, RenderTargetCount, UseColorTexture, UseDepthTexture, UseStencil)) return false;
		}

		m_OffScreenRenderPassMap.insert({ PassName, RenderPass });

		return true;
	}

	std::shared_ptr<graphics::CVertexBuffer> COpenGLAPI::CreateVertexBuffer()
	{
		auto VertexBuffer = std::make_shared<api::COpenGLVertexBuffer>(this);

		return VertexBuffer;
	}

	std::shared_ptr<graphics::CIndexBuffer> COpenGLAPI::CreateIndexBuffer()
	{
		auto IndexBuffer = std::make_shared<api::COpenGLIndexBuffer>(this);

		return IndexBuffer;
	}

	std::shared_ptr<graphics::IRenderer> COpenGLAPI::CreateRenderer()
	{
		auto Renderer = std::make_shared<api::COpenGLRenderer>(this);

		return Renderer;
	}

	std::shared_ptr<graphics::CMaterial> COpenGLAPI::CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode)
	{
		auto Material = std::make_shared<api::COpenGLMaterial>(this, createInfo, CullMode);

		return Material;
	}

	std::shared_ptr<graphics::CTexture> COpenGLAPI::CreateTexture(bool UseMipMap, graphics::STextureSamplerParam SamplerParam)
	{
		auto Texture = std::make_shared<api::COpenGLTexture>(this, UseMipMap, SamplerParam);

		return Texture;
	}

#ifdef USE_GPGPU
	std::shared_ptr<api::IGPGPUHandler> COpenGLAPI::CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial)
	{
		auto GPGPUHandler = std::make_shared<api::COpenGLGPGPUHandler>(this, ComputeMaterial);

		return GPGPUHandler;
	}
#endif // USE_GPGPU

	bool COpenGLAPI::Resize(int Width, int Height)
	{
		m_Width = Width;
		m_Height = Height;

		return true;
	}

	bool COpenGLAPI::PrepareRender()
	{
		return true;
	}

	bool COpenGLAPI::BeginRender(const std::string& PassName)
	{

		// レンダーパスを切り替える
		const auto& OffScreenRenderPass = m_OffScreenRenderPassMap.find(PassName);
		if (OffScreenRenderPass != m_OffScreenRenderPassMap.end())
		{
			if (!OffScreenRenderPass->second->BeginRenderPass()) return false;
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, m_Width, m_Height);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClearDepth(1.0f);
			glClearStencil(0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			// glStencilMaskはglColorMask・glDepthMaskと同じ関数でフレームバッファへの書き込みを有効にしたり無効にしたりする
			// 0xFFにすることで有効になる?
			glStencilMask(0xff);
		}

		m_CurrentRenderPassName = PassName;

		return true;
	}

	bool COpenGLAPI::EndRender()
	{
		return true;
	}

	bool COpenGLAPI::SubmitRender()
	{
		return true;
	}

	std::string COpenGLAPI::GetVertexShaderExtension() const
	{
		return m_VertexShaderExtension;
	}

	std::string COpenGLAPI::GetFragmentShaderExtension() const
	{
		return m_FragmentShaderExtension;
	}

	std::string COpenGLAPI::GetGeometryShaderExtension() const
	{
		return m_GeometryShaderExtension;
	}

	std::string COpenGLAPI::GetHullShaderExtension() const
	{
		return m_HullShaderExtension;
	}

	std::string COpenGLAPI::GetDomainShaderExtension() const
	{
		return m_DomainShaderExtension;
	}

	std::string COpenGLAPI::GetComputeShaderExtension() const
	{
		return m_ComputeShaderExtension;
	}


	int COpenGLAPI::GetWidth() const
	{
		return m_Width;
	}

	int COpenGLAPI::GetHeight() const
	{
		return m_Height;
	}

	std::shared_ptr<graphics::IRenderPass> COpenGLAPI::FindOffScreenRenderPass(const std::string& PassName)
	{
		auto it = m_OffScreenRenderPassMap.find(PassName);
		if (it == m_OffScreenRenderPassMap.end()) return nullptr;

		return it->second;
	}

	bool COpenGLAPI::CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName)
	{
		GLuint SrcFrameBuffer = GetFrameBuffer(SrcPassName);
		GLuint DstFrameBuffer = GetFrameBuffer(DstPassName);

		if (SrcFrameBuffer == -1 || DstFrameBuffer == -1) return false;

		//
		const auto& it = m_OffScreenRenderPassMap.find(SrcPassName);
		if (it == m_OffScreenRenderPassMap.end()) return false;
		COpenGLRenderPass* pSrcRenderPass = static_cast<COpenGLRenderPass*>(it->second.get());

		int Width = pSrcRenderPass->GetFrameTexture()->GetWidth();
		int Height = pSrcRenderPass->GetFrameTexture()->GetHeight();

		//
		glBindFramebuffer(GL_READ_FRAMEBUFFER, SrcFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DstFrameBuffer);

		glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	bool COpenGLAPI::CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName)
	{
		GLuint SrcFrameBuffer = GetFrameBuffer(SrcPassName);
		GLuint DstFrameBuffer = GetFrameBuffer(DstPassName);

		if (SrcFrameBuffer == -1 || DstFrameBuffer == -1) return false;

		//
		const auto& it = m_OffScreenRenderPassMap.find(SrcPassName);
		if (it == m_OffScreenRenderPassMap.end()) return false;
		COpenGLRenderPass* pSrcRenderPass = static_cast<COpenGLRenderPass*>(it->second.get());

		int Width = pSrcRenderPass->GetDepthTexture()->GetWidth();
		int Height = pSrcRenderPass->GetDepthTexture()->GetHeight();

		//
		glBindFramebuffer(GL_READ_FRAMEBUFFER, SrcFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, DstFrameBuffer);

		glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	GLuint COpenGLAPI::GetFrameBuffer(const std::string& PassName)
	{
		if (PassName.empty())
		{
			// デフォルトフレームバッファを返す
			return 0;
		}
		else
		{
			const auto& it = m_OffScreenRenderPassMap.find(PassName);
			if (it == m_OffScreenRenderPassMap.end()) return -1;

			COpenGLRenderPass* pRenderPass = static_cast<COpenGLRenderPass*>(it->second.get());
			return pRenderPass->GetFrameBuffer();
		}
	}

	bool COpenGLAPI::IsEnabledRuntimeShaderEditing() const
	{
#ifdef RUNTIME_SHADER_EDITING
		return true;
#else
		return false;
#endif // RUNTIME_SHADER_EDITING
	}

	bool COpenGLAPI::CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage)
	{
		GLuint shader = 0;

		const bool Result = COpenGLMaterial::CompileShader(ShaderCode, ShaderStage, shader, ErrorMsg);

		if (shader != 0) glDeleteShader(shader);

		return Result;
	}
}

#endif // USE_OPENGL