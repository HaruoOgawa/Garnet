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

	void* CImGuiCoreWebGPU::CastTexID(graphics::CTexture* pTexture)
	{
		api::CWebGPUTexture* pWebGPUTexture = static_cast<api::CWebGPUTexture*>(pTexture);

		return (void*)(intptr_t)pWebGPUTexture->GetTextureImageView();
	}
}
#endif