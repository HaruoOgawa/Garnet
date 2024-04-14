#if defined(USE_OPENGL) and defined(USE_GUIENGINE)
#include "CImGuiCoreOpenGL.h"

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
}
#endif