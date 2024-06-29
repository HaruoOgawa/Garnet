#if defined(USE_OPENGL) and defined(USE_GUIENGINE)
#include "CImGuiCoreOpenGL.h"
#include "../../GraphicsAPI/OpenGL/COpenGLTexture.h"

namespace gui
{
	CImGuiCoreOpenGL::CImGuiCoreOpenGL()
	{
	}

	CImGuiCoreOpenGL::~CImGuiCoreOpenGL()
	{
	}

	void CImGuiCoreOpenGL::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}

	bool CImGuiCoreOpenGL::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplOpenGL3_Init();

		return true;
	}

	bool CImGuiCoreOpenGL::BeginFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplOpenGL3_NewFrame();

		return true;
	}

	bool CImGuiCoreOpenGL::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		return true;
	}

	bool CImGuiCoreOpenGL::DrawTexture(graphics::CTexture* pTexture, const glm::vec2& Size, const glm::vec2& UV0, const glm::vec2& UV1)
	{
		api::COpenGLTexture* pOpenGLTexture = static_cast<api::COpenGLTexture*>(pTexture);

		ImGui::Image((void*)(intptr_t)pOpenGLTexture->GetTextureID(), ImVec2(Size.x, Size.y), ImVec2(UV0.x, UV0.y), ImVec2(UV1.x, UV1.y));

		return true;
	}
}
#endif