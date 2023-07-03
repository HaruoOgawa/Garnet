#pragma once

#ifndef __DAWN__

#include <string>

#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw3.h>
#include <glfw3native.h>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"

namespace graphics { class CTexture; }

namespace api
{
	class CVulkanAPI;
	class CVulkanTexture;

	class CVulkanRenderPass : public graphics::IRenderPass
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		// Base Param
		std::string m_PassName;
		int m_Width;
		int m_Height;
		api::ERenderPassFormat m_RenderPassFormat;
		std::shared_ptr<CVulkanTexture> m_FrameTexture;

		// Command
		VkCommandPool   m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		// Rendering
		VkRenderPass m_RenderPass;
		VkFramebuffer m_FrameBuffer;

		// Depth Test
		VkImage m_DepthImage;
		VkDeviceMemory m_DepthImageMemory;
		VkImageView m_DepthImageView;
	private:
		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();

		bool CreateRenderPass();
		bool CreateDepthResources();
		bool CreateFrameBuffer();

		bool CreateCommandPool();
		bool CreateCommandBuffer();
	public:
		CVulkanRenderPass(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName, int Width, int Height, ERenderPassFormat RenderPassFormat);
		virtual ~CVulkanRenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture() override;
		VkRenderPass GetRenderPass() const { return m_RenderPass; }
		VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

		bool Create();

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;
	};
}
#endif