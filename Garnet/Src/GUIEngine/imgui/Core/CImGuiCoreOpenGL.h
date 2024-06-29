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

		virtual void* CastTexID(graphics::CTexture* pTexture) override;
	};
}
#endif