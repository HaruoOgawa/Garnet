#ifdef USE_OPENGL

#include "COpenGLAPI.h"

#define GL_IMPLEMENTATION
#include "glDef.h"

#include "COpenGLMaterial.h"
#include "COpenGLTexture.h"
#include "COpenGLRenderer.h"
#include "COpenGLRenderPass.h"
#include "COpenGLGPGPUHandler.h"

namespace api
{
	COpenGLAPI::COpenGLAPI(int Width, int Height):
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

	bool COpenGLAPI::Initialize()
	{
		if (!InitGL()) return false;
		
		return true;
	}

	void COpenGLAPI::Release()
	{
	}

	bool COpenGLAPI::CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width, int Height)
	{
		std::shared_ptr<COpenGLRenderPass> RenderPass = std::make_shared<COpenGLRenderPass>(this, PassName, RenderPassFormat, InitColor);

		if (Width != -1 && Height != -1)
		{
			if (!RenderPass->Create(Width, Height)) return false;
		}
		else
		{
			if (!RenderPass->Create(m_Width, m_Height)) return false;
		}

		m_OffScreenRenderPassMap.insert({ PassName, RenderPass });

		return true;
	}

	std::shared_ptr<renderer::IRenderer> COpenGLAPI::CreateRenderer(const std::string& PassName)
	{
		auto Renderer = std::make_shared<renderer::COpenGLRenderer>(this, PassName);

		return Renderer;
	}

	std::shared_ptr<graphics::CMaterial> COpenGLAPI::CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo)
	{
		auto Material = std::make_shared<api::COpenGLMaterial>(this, createInfo);

		return Material;
	}

	std::shared_ptr<graphics::CTexture> COpenGLAPI::CreateTexture(bool UseMipMap)
	{
		auto Texture = std::make_shared<api::COpenGLTexture>(this, UseMipMap);

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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

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

	const std::string& COpenGLAPI::GetVertexShaderExtension() const
	{
		return m_VertexShaderExtension;
	}

	const std::string& COpenGLAPI::GetFragmentShaderExtension() const
	{
		return m_FragmentShaderExtension;
	}

	const std::string& COpenGLAPI::GetGeometryShaderExtension() const
	{
		return m_GeometryShaderExtension;
	}

	const std::string& COpenGLAPI::GetHullShaderExtension() const
	{
		return m_HullShaderExtension;
	}

	const std::string& COpenGLAPI::GetDomainShaderExtension() const
	{
		return m_DomainShaderExtension;
	}

	const std::string& COpenGLAPI::GetComputeShaderExtension() const
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

	const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& COpenGLAPI::GetOffScreenRenderPassMap() const
	{
		return m_OffScreenRenderPassMap;
	}
}

#endif // USE_OPENGL