#if defined(USE_WEBGPU) and defined(USE_GUIENGINE)
#include "CImGuiCoreWebGPU.h"
#include "../../GraphicsAPI/WebGPU/CWebGPUTexture.h"

namespace gui
{
	CImGuiCoreWebGPU::CImGuiCoreWebGPU()
	{
	}

	CImGuiCoreWebGPU::~CImGuiCoreWebGPU()
	{
	}

	void CImGuiCoreWebGPU::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CWebGPUAPI* pWebGPUApi = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		ImGui_ImplWGPU_Shutdown();
	}

	bool CImGuiCoreWebGPU::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CWebGPUAPI* pWebGPUApi = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		// WebGPU‚ÌÝ’è
		ImGui_ImplWGPU_InitInfo init_info = {};
		init_info.Device = pWebGPUApi->GetLogicalDevice();
		init_info.NumFramesInFlight = 2;
		init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
		init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;

		ImGui_ImplWGPU_Init(&init_info);

		return true;
	}

	bool CImGuiCoreWebGPU::BeginFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplWGPU_NewFrame();

		return true;
	}

	bool CImGuiCoreWebGPU::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CWebGPUAPI* pWebGPUApi = static_cast<api::CWebGPUAPI*>(pGraphicsAPI);

		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pWebGPUApi->GetSwapChainRenderPass());

		return true;
	}

	bool CImGuiCoreWebGPU::DrawTexture(graphics::CTexture* pTexture, const glm::vec2& Size, const glm::vec2& UV0, const glm::vec2& UV1)
	{
		api::CWebGPUTexture* pWebGPUTexture = static_cast<api::CWebGPUTexture*>(pTexture);

		ImGui::Image((void*)(intptr_t)pWebGPUTexture->GetTextureImageView(), ImVec2(Size.x, Size.y), ImVec2(UV0.x, UV0.y), ImVec2(UV1.x, UV1.y));

		return true;
	}
}
#endif