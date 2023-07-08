#ifndef __DAWN__
#include "CVulkanRenderPass.h"
#include "CVulkanAPI.h"
#include "CVulkanTexture.h"
#include "../../Graphics/ETextureType.h"
#include "../../Graphics/ETextureType.h"
#include "../../Debug/Message/Console.h"

namespace api
{
	CVulkanRenderPass::CVulkanRenderPass(api::CVulkanAPI* pGraphicsAPI, const std::string& PassName, ERenderPassFormat RenderPassFormat):
		m_pGraphicsAPI(pGraphicsAPI),
		
		m_PassName(PassName),
		m_Width(0),
		m_Height(0),
		m_RenderPassFormat(RenderPassFormat),
		m_FrameTexture(nullptr),
		
		m_CommandPool(nullptr),
		m_CommandBuffer(nullptr),

		m_RenderPass(nullptr),
		m_FrameBuffer(nullptr),

		m_DepthImage(nullptr),
		m_DepthImageMemory(nullptr),
		m_DepthImageView(nullptr)
	{
	}

	CVulkanRenderPass::~CVulkanRenderPass()
	{
		if (m_CommandPool)
		{
			vkDestroyCommandPool(m_pGraphicsAPI->GetLogicalDevice(), m_CommandPool, nullptr);
			m_CommandPool = nullptr;
		}

		// デプスリソースを破棄
		if (m_DepthImageView)
		{
			vkDestroyImageView(m_pGraphicsAPI->GetLogicalDevice(), m_DepthImageView, nullptr);
			m_DepthImageView = nullptr;
		}
		
		if (m_DepthImage)
		{
			vkDestroyImage(m_pGraphicsAPI->GetLogicalDevice(), m_DepthImage, nullptr);
			m_DepthImage = nullptr;
		}
		
		if (m_DepthImageMemory)
		{
			vkFreeMemory(m_pGraphicsAPI->GetLogicalDevice(), m_DepthImageMemory, nullptr);
			m_DepthImageMemory = nullptr;
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

	std::shared_ptr<graphics::CTexture> CVulkanRenderPass::GetFrameTexture()
	{
		return m_FrameTexture;
	}

	bool CVulkanRenderPass::Create(int Width, int Height)
	{
		m_Width = Width;
		m_Height = Height;

		m_FrameTexture = std::make_shared<CVulkanTexture>(m_pGraphicsAPI, false);
		if (!m_FrameTexture->CreateFrameTexture(Width, Height, m_RenderPassFormat)) return false;

		if (!CreateRenderPass()) return false; // レンダーパスの作成(描画全体のマネージャー。実際に描画に使用するのがサブパス。サブパスを複数個用意することでポストプロセスもできる)
		if (!CreateDepthResources(Width, Height)) return false; // デプステスト用のリソースを生成
		if (!CreateFrameBuffer(Width, Height)) return false; // フレームバッファの作成
		if (!CreateCommandPool()) return false;
		if (!CreateCommandBuffer()) return false;

		return true;
	}

	bool CVulkanRenderPass::CreateRenderPass()
	{
		//
		std::vector<VkAttachmentDescription> attachments;

		// <カラーバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		if (m_RenderPassFormat != ERenderPassFormat::DEPTH_ONLY_RENDERPASS && m_RenderPassFormat != ERenderPassFormat::DEPTH_ONLY_FLOAT_RENDERPASS)
		{
			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = (m_RenderPassFormat == ERenderPassFormat::COLOR_DEPTH_FLOAT_RENDERPASS) ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM;
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
		VkAttachmentReference colorAttachmentRef{}; // 前のパスの参照方法の定義(かな？)
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

		// <デプスバッファ> ////////////////////////////////////////////////////////////////
		// レンダーパスの基本的な設定
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = m_pGraphicsAPI->FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // マルチサンプリング
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // レンダリングの前後にどのような処理を施すか(クリアの方法など)。デプスバッファに適応
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // レンダリング結果をメモリに保存し読み取り可にする。デプスバッファに適応
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応。 DONT_CAREは何もしない
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 上記の設定をステンシルバッファに適応
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // レンダリング前にどのようなレイアウトとして使用するか
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // レンダリング後にどのようなレイアウトとして使用するか

		attachments.push_back(depthAttachment);

		// サブパスの設定(サブパスとは前のパスのフレームバッファの内容を参照するレンダリング操作。ポストプロセスなどに有用)
		VkAttachmentReference depthAttachmentRef{}; // 前のパスの参照方法の定義(かな？)
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // カラーレンダーバッファとして使用することを明示???

		///////////////////////////////////////////////////////////////////////////////////
		//
		VkSubpassDescription subpass{}; // 実際に使用するサブパスの設定
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // グラフィック用のサブパスであることを指定する
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef; // 参照方法について
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

	bool CVulkanRenderPass::CreateDepthResources(int Width, int Height)
	{
		VkFormat depthFormat = m_pGraphicsAPI->FindDepthFormat();
		m_pGraphicsAPI->CreateImage(Width, Height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory, graphics::ETextureType::TEXTURE_2D, 1, false);

		m_DepthImageView = m_pGraphicsAPI->CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, graphics::ETextureType::TEXTURE_2D, 1, false);

		return true;
	}

	bool CVulkanRenderPass::CreateFrameBuffer(int Width, int Height)
	{
		std::array<VkImageView, 2> attachments[] = {
				m_FrameTexture->GetTextureImageView(),
				m_DepthImageView
		};

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = m_RenderPass;
		frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments->size());
		frameBufferInfo.pAttachments = attachments->data();
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

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
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

	bool CVulkanRenderPass::CreateCommandPool()
	{
		QueueFamiryIndices queueFamilyIndices = m_pGraphicsAPI->FindQueueFamilies(m_pGraphicsAPI->GetPhysicalDevice());

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily.value();

		if (vkCreateCommandPool(m_pGraphicsAPI->GetLogicalDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	bool CVulkanRenderPass::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // メイン(プライマリ)コマンドバッファかサブ(セカンダリ)コマンドバッファかの選択
		allocInfo.commandBufferCount = 1;

		// Allocate は確保するという意味
		if (vkAllocateCommandBuffers(m_pGraphicsAPI->GetLogicalDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}
}
#endif
