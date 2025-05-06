#pragma once

#ifdef USE_VULKAN

#include <string>

#include <glfw3.h>
#include <glfw3native.h>

#include "../ERenderPassFormat.h"
#include "../../Interface/IRenderPass.h"
#include <glm/glm.hpp>

namespace graphics { class CTexture; }

namespace api
{
	class CVulkanAPI;
	class CVulkanTexture;

	class CVulkanRenderPass : public graphics::IRenderPass
	{
		// API
		api::CVulkanAPI* m_pGraphicsAPI;

		graphics::SRenderPassState m_PassState;

		// Base Param
		std::string m_PassName;
		int m_Width;
		int m_Height;
		glm::vec4 m_InitColor;
		api::ERenderPassFormat m_RenderPassFormat_Color;
		api::ERenderPassFormat m_RenderPassFormat_Depth;
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;
		std::shared_ptr<graphics::CTexture> m_DepthTexture;
		std::vector<std::shared_ptr<graphics::CTexture>> m_MSAAResolveTextureList;

		// Command
		VkCommandPool   m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		// Rendering
		VkRenderPass m_RenderPass;
		VkFramebuffer m_FrameBuffer;
	private:
		bool BeginRecordCommandBuffer();
		bool EndRecordCommandBuffer();

		bool CreateRenderPass(const graphics::SRenderPassState& PassState);
		bool CreateFrameBuffer(int Width, int Height);
	public:
		CVulkanRenderPass(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor);
		virtual ~CVulkanRenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture(int Index = 0) override;
		virtual const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const override;
		virtual const std::shared_ptr<graphics::CTexture>& GetDepthTexture() const override;

		const graphics::SRenderPassState& GetPassState() const;

		VkRenderPass GetRenderPass() const { return m_RenderPass; }
		VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffer; }

		bool Create(int Width, int Height, const graphics::SRenderPassState& PassState) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
	};
}
#endif