#pragma once

#if defined(USE_WEBGPU) and defined(USE_GUIENGINE)

#include "IImGuiCore.h"
#include <imgui_impl_wgpu.h>
#include "../../GraphicsAPI/WebGPU/CWebGPUAPI.h"

namespace gui
{
	class CImGuiCoreWebGPU : public IImGuiCore
	{
	public:
		CImGuiCoreWebGPU();
		virtual ~CImGuiCoreWebGPU();

		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual void* CastTexID(graphics::CTexture* pTexture) override;
	};
}
#endif