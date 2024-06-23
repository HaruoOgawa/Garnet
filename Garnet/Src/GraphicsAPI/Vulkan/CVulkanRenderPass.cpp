#ifdef USE_VULKAN
#include "CVulkanRenderPass.h"
#include "CVulkanAPI.h"
#include "CVulkanTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Message/Console.h"

namespace api
{
	CVulkanRenderPass::CVulkanRenderPass(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor):
		m_pGraphicsAPI(pGraphicsAPI),
		m_RenderTargetCount(1),
		m_PassName(PassName),
		m_Width(0),
		m_Height(0),
		m_InitColor(InitColor),
		m_RenderPassFormat(RenderPassFormat),
		m_DepthTexture(nullptr),
		
		m_CommandPool(nullptr),
		m_CommandBuffer(nullptr),

		m_RenderPass(nullptr),
		m_FrameBuffer(nullptr)
	{
	}

	CVulkanRenderPass::~CVulkanRenderPass()
	{
		m_FrameTextureList.clear();
		m_DepthTexture = nullptr;

		if (m_CommandPool)
		{
			vkDestroyCommandPool(m_pGraphicsAPI->GetLogicalDevice(), m_CommandPool, nullptr);
			m_CommandPool = nullptr;
		}
		
		// フレームバッファの破棄
		if (m_FrameBuffer)
		{
			vkDestroyFramebuffer(m_pGraphicsAPI->GetLogicalDevice(), m_FrameBuffer, nullptr);
			m_FrameBuffer = nullptr;
		}

		// レンダーパスの破棄
		if (m_RenderPass)
		{
			vkDestroyRenderPass(m_pGraphicsAPI->GetLogicalDevice(), m_RenderPass, nullptr);
			m_RenderPass = nullptr;
		}
	}

	std::shared_ptr<graphics::CTexture> CVulkanRenderPass::GetFrameTexture(int Index)
	{
		if (Index < 0 || Index >= static_cast<int>(m_FrameTextureList.size())) return nullptr;

		return m_FrameTextureList[Index];
	}

	const std::vector<std::shared_ptr<graphics::CTexture>>& CVulkanRenderPass::GetFrameTextureList() const
	{
		return m_FrameTextureList;
	}
	
	const std::shared_ptr<graphics::CTexture>& CVulkanRenderPass::GetDepthTexture() const
	{
		return m_DepthTexture;
	}

	bool CVulkanRenderPass::Create(int Width, int Height, int RenderTargetCount)
	{
		m_Width = Width;
		m_Height = Height;
		m_RenderTargetCount = RenderTargetCount;

		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++)
		{
			auto FrameTexture = m_pGraphicsAPI->CreateTexture(false);
			if (!FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;

			m_FrameTextureList.push_back(FrameTexture);
		}
		
		m_DepthTexture = std::make_shared<CVulkanTexture>(m_pGraphicsAPI, false);
		if (!m_DepthTexture->CreateFrameTexture(Width, Height, api::ERenderPassFormat::DEPTH_RENDERPASS)) return false;

		if (!CreateRenderPass(RenderTargetCount)) return false; // レンダーパスの作成(描画全体のマネージャー。実際に描画に使用するのがサブパス。サブパスを複数個用意することでポストプロセスもできる)
		if (!CreateFrameBuffer(Width, Height)) return false; // フレームバッファの作成
		if (!m_pGraphicsAPI->CreateCommandPool(m_CommandPool)) return false;
		if (!m_pGraphicsAPI->CreateCommandBuffer(m_CommandBuffer, m_CommandPool)) return false;

		return true;
	}

	bool CVulkanRenderPass::CreateRenderPass(int RenderTargetCount)
	{
		//
		std::vector<VkAttachmentDescription> attachments;

		// <カラーバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++)
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = (m_RenderPassFormat == ERenderPassFormat::COLOR_FLOAT_RENDERPASS) ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリング
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // レンダリングの前後にどのような処理を施すか(クリアの方法など)
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // レンダリング結果をメモリに保存し読み取り可にする
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応。 DONT_CAREは何もしない
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // レンダリング前にどのようなレイアウトとして使用するか
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // レンダリング後にどのようなレイアウトとして使用するか

			attachments.push_back(colorAttachment);
		}

		// サブパスの設定(サブパスとは前のパスのフレームバッファの内容を参照するレンダリング操作。ポストプロセスなどに有用)
		std::vector<VkAttachmentReference> colorAttachmentRefs; // 前のパスの参照方法の定義(かな？)
		for (int AttachmentIndex = 0; AttachmentIndex < RenderTargetCount; AttachmentIndex++)
		{
			VkAttachmentReference attachmentRef{};

			attachmentRef.attachment = AttachmentIndex;
			attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

			colorAttachmentRefs.push_back(attachmentRef);
		}

		// <デプスバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = m_pGraphicsAPI->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリング
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // レンダリングの前後にどのような処理を施すか(クリアの方法など)。デプスバッファに適応
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // レンダリング結果をメモリに保存し読み取り可にする。デプスバッファに適応
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応。 DONT_CAREは何もしない
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // レンダリング前にどのようなレイアウトとして使用するか
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // レンダリング後にどのようなレイアウトとして使用するか

		attachments.push_back(depthAttachment);

		// サブパスの設定(サブパスとは前のパスのフレームバッファの内容を参照するレンダリング操作。ポストプロセスなどに有用)
		VkAttachmentReference depthAttachmentRef{}; // 前のパスの参照方法の定義(かな？)
		uint32_t depthIndex = static_cast<uint32_t>(colorAttachmentRefs.size());
		depthAttachmentRef.attachment = depthIndex;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

		///////////////////////////////////////////////////////////////////////////////////
		//
		VkSubpassDescription subpass{}; // 実際に使用するサブパスの設定
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // グラフィック用のサブパスであることを指定する
		subpass.colorAttachmentCount = RenderTargetCount;
		subpass.pColorAttachments = &colorAttachmentRefs[0]; // 参照方法について
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		// レンダーパスの作成
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		// サブパスの依存関係を設定(よくわからん)
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_pGraphicsAPI->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			Console::Log("[ERROR] failed to create render pass!\n");

			return false;
		}

		return true;
	}

	bool CVulkanRenderPass::CreateFrameBuffer(int Width, int Height)
	{
		std::vector<VkImageView> attachments;

		for (const auto& FrameTexture : m_FrameTextureList)
		{
			CVulkanTexture* pVulkanTexture = static_cast<CVulkanTexture*>(FrameTexture.get());

			attachments.push_back(pVulkanTexture->GetTextureImageView());
		}

		attachments.push_back(static_cast<CVulkanTexture*>(m_DepthTexture.get())->GetTextureImageView());

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = m_RenderPass;
		frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		frameBufferInfo.pAttachments = &attachments[0];
		frameBufferInfo.width = Width;
		frameBufferInfo.height = Height;
		frameBufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_pGraphicsAPI->GetLogicalDevice(), &frameBufferInfo, nullptr, &m_FrameBuffer) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanRenderPass::BeginRenderPass()
	{
		// コマンドバッファの記録開始
		if (!BeginRecordCommandBuffer()) return false;

		// レンダーパス開始 
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_FrameBuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = {static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height)};
		
		std::vector<VkClearValue> clearValues;

		for (int AttachmentIndex = 0; AttachmentIndex < m_RenderTargetCount; AttachmentIndex++)
		{
			VkClearValue clearValue{};
			clearValue.color = { {m_InitColor.x, m_InitColor.y, m_InitColor.z, m_InitColor.w} };

			clearValues.push_back(clearValue);
		}
		
		{
			VkClearValue clearValue{};
			clearValue.depthStencil = { 1.0f, 0 };

			clearValues.push_back(clearValue);
		}

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// レンダーパス開始コマンドを発行
		vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// 動的指定にしたビューポートとシザーの設定をここで行う(ウィンドウのリサイズにとても役立つやつ)
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_Width);
		viewport.height = static_cast<float>(m_Height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport); // ビューポート再設定用のコマンドを発行

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { static_cast<unsigned int>(m_Width), static_cast<unsigned int>(m_Height) };
		vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor); // シザーの再設定用のコマンドを発行

		return true;
	}

	bool CVulkanRenderPass::EndRenderPass()
	{
		// レンダーパス終了
		vkCmdEndRenderPass(m_CommandBuffer);

		// コマンドバッファの記録を終了
		if (!EndRecordCommandBuffer()) return false;

		// コマンドバッファの送信
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;

		// コマンドバッファをグラフィックキューに送信
		// コマンドバッファにはコマンドが入っていてそのコマンドをキューが実行する
		// キューはタスクでその具体的なタスク内容がコマンドという理解もできる
		// レンダーパスへの描画コマンドを実行する

		// 描画が終わるまでフェンスで次の処理を待たせる
		const auto& Fence = m_pGraphicsAPI->GetInFlightFence();

		if (vkQueueSubmit(m_pGraphicsAPI->GetGraphicsQueue(), 1, &submitInfo, Fence) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanRenderPass::BeginRecordCommandBuffer()
	{
		// 前のフレームの処理が終わるのを待つ
		const auto& Fence = m_pGraphicsAPI->GetInFlightFence();
		vkWaitForFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence, VK_TRUE, UINT32_MAX);

		// 処理が終わったのでフェンスをリセットしてまた使える状態にしておく
		vkResetFences(m_pGraphicsAPI->GetLogicalDevice(), 1, &Fence);

		// コマンドバッファをリセットする
		vkResetCommandBuffer(m_CommandBuffer, 0);

		// コマンドバッファの記録開始
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanRenderPass::EndRecordCommandBuffer()
	{
		// コマンドバッファの記録を終了
		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}
#endif
