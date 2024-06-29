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

		virtual bool DrawTexture(graphics::CTexture* pTexture, const glm::vec2& Size, const glm::vec2& UV0 = glm::vec2(0.0f, 0.0f), const glm::vec2& UV1 = glm::vec2(1.0f, 1.0f)) override;
	};
}
#endif