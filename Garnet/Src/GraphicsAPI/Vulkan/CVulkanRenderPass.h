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
		api::ERenderPassFormat m_RenderPassFormat_Color;
		api::ERenderPassFormat m_RenderPassFormat_Depth;
		std::vector<std::shared_ptr<graphics::CTexture>> m_FrameTextureList;
		std::vector<std::shared_ptr<graphics::CTexture>> m_ResolveTextureList;
		std::shared_ptr<graphics::CTexture> m_DepthTexture;

		// Rendering
		VkRenderPass m_RenderPass;
		VkFramebuffer m_FrameBuffer;
	private:
		bool CreateRenderPass(const graphics::SRenderPassState& PassState);
		bool CreateFrameBuffer(int Width, int Height);
	public:
		CVulkanRenderPass(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat);
		virtual ~CVulkanRenderPass();

		virtual std::shared_ptr<graphics::CTexture> GetFrameTexture(int Index = 0) override;
		virtual const std::vector<std::shared_ptr<graphics::CTexture>>& GetFrameTextureList() const override;
		virtual const std::shared_ptr<graphics::CTexture>& GetDepthTexture() const override;

		const graphics::SRenderPassState& GetPassState() const;

		VkRenderPass GetRenderPass() const { return m_RenderPass; }

		bool Create(int Width, int Height, const graphics::SRenderPassState& PassState) override;

		virtual bool BeginRenderPass() override;
		virtual bool EndRenderPass() override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;
	};
}
#endif