#ifdef USE_OPENGL

#include "COpenGLAPI.h"

#define GL_IMPLEMENTATION
#include "glDef.h"

namespace api
{
	COpenGLAPI::COpenGLAPI(int Width, int Height):
		m_ShaderExtension(".glsl"),
		m_Width(Width),
		m_Height(Height)
	{
	}

	COpenGLAPI::~COpenGLAPI()
	{
	}

	bool COpenGLAPI::InitializeWithGLFW(GLFWwindow* pWindow)
	{
		if (!InitGL()) return false;
		
		return true;
	}

	void COpenGLAPI::Release()
	{
	}

	bool COpenGLAPI::CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width, int Heigh)
	{
		return true;
	}

	std::shared_ptr<renderer::IRenderer> COpenGLAPI::CreateRenderer(const std::string& PassName)
	{
		return nullptr;
	}

	std::shared_ptr<graphics::CMaterial> COpenGLAPI::CreateMaterial()
	{
		return nullptr;
	}

	std::shared_ptr<graphics::CTexture> COpenGLAPI::CreateTexture(bool UseMipMap)
	{
		return nullptr;
	}

	bool COpenGLAPI::Resize(int Width, int Height)
	{
		return true;
	}

	bool COpenGLAPI::PrepareRender()
	{
		return true;
	}

	bool COpenGLAPI::BeginRender(const std::string& PassName)
	{
		glBindBuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_Width, m_Height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	const std::string& COpenGLAPI::GetShaderExtension() const
	{
		return m_ShaderExtension;
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