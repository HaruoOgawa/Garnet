#pragma once

#if defined(USE_VULKAN) and defined(USE_GUIENGINE)

#include "IImGuiCore.h"
#include <imgui_impl_vulkan.h>
#include "../../GraphicsAPI/Vulkan/CVulkanAPI.h"

namespace gui
{
	class CImGuiCoreVulkan : public IImGuiCore
	{
		VkDescriptorPool m_ImGuiDescriptorPool;
	private:
		static void CheckVkResult(VkResult err);

		bool CreateDescriptorPool(api::CVulkanAPI* pVulkanAPI);

	public:
		CImGuiCoreVulkan();
		virtual ~CImGuiCoreVulkan();

		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) override;

		virtual void* CastTexID(graphics::CTexture* pTexture) override;
	};
}
#endif