#pragma once

#if defined(USE_OPENGL) and defined(USE_GUIENGINE)

#include "IImGuiCore.h"
#include <imgui_impl_opengl3.h>
#include "../../GraphicsAPI/OpenGL/COpenGLAPI.h"

namespace gui
{
	class CImGuiCoreOpenGL : public IImGuiCore
	{
	public:
		CImGuiCoreOpenGL();
		virtual ~CImGuiCoreOpenGL();

		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool DrawTexture(graphics::CTexture* pTexture, const glm::vec2& Size, const glm::vec2& UV0 = glm::vec2(0.0f, 0.0f), const glm::vec2& UV1 = glm::vec2(1.0f, 1.0f)) override;
	};
}
#endif