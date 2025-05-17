#if defined(USE_VULKAN) and defined(USE_GUIENGINE)
#include "CImGuiCoreVulkan.h"
#include "../../Message/Console.h"
#include "../../GraphicsAPI/Vulkan/CVulkanTexture.h"

namespace gui
{
	CImGuiCoreVulkan::CImGuiCoreVulkan():
		m_ImGuiDescriptorPool(VK_NULL_HANDLE)
	{
	}

	CImGuiCoreVulkan::~CImGuiCoreVulkan()
	{
	}

	void CImGuiCoreVulkan::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CVulkanAPI* pVulkanAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);

		if (m_ImGuiDescriptorPool)
		{
			vkDestroyDescriptorPool(pVulkanAPI->GetLogicalDevice(), m_ImGuiDescriptorPool, nullptr);
			m_ImGuiDescriptorPool = nullptr;
		}

		ImGui_ImplVulkan_Shutdown();
	}

	bool CImGuiCoreVulkan::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CVulkanAPI* pVulkanAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);

		// 使用可能なキューを取得
		api::QueueFamiryIndices indices = pVulkanAPI->FindQueueFamilies(pVulkanAPI->GetPhysicalDevice());
		auto GraphicsQueue = indices.m_GraphicsAndComputeFamily.value();

		// ImGui全体で使いまわす用のDescriptorPoolを作成
		if (!CreateDescriptorPool(pVulkanAPI)) return false;

		// Vulkanの設定
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = pVulkanAPI->GetInstance();
		init_info.PhysicalDevice = pVulkanAPI->GetPhysicalDevice();
		init_info.Device = pVulkanAPI->GetLogicalDevice();
		init_info.QueueFamily = GraphicsQueue;
		init_info.Queue = pVulkanAPI->GetGraphicsQueue();
		init_info.PipelineCache = nullptr;
		init_info.DescriptorPool = m_ImGuiDescriptorPool;
		init_info.RenderPass = pVulkanAPI->GetSwapChainRenderPass();
		init_info.Subpass = 0;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = CheckVkResult;

		ImGui_ImplVulkan_Init(&init_info);

		return true;
	}

	bool CImGuiCoreVulkan::BeginFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		ImGui_ImplVulkan_NewFrame();

		return true;
	}

	bool CImGuiCoreVulkan::EndFrame(api::IGraphicsAPI* pGraphicsAPI)
	{
		api::CVulkanAPI* pVulkanAPI = static_cast<api::CVulkanAPI*>(pGraphicsAPI);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), pVulkanAPI->GetCurrentCommandBuffer());

		return true;
	}

	void* CImGuiCoreVulkan::CastTexID(graphics::CTexture* pTexture)
	{
		api::CVulkanTexture* pVulkanTexture = static_cast<api::CVulkanTexture*>(pTexture);

		return (void*)(intptr_t)pVulkanTexture->GetGUIDescriptorSet();
	}

	void CImGuiCoreVulkan::CheckVkResult(VkResult err)
	{
		if (err == 0)
		{
			return;
		}
		else
		{
			Console::Log("[ImGui Error] err: %d\n", err);
		}
	}

	bool CImGuiCoreVulkan::CreateDescriptorPool(api::CVulkanAPI* pVulkanAPI)
	{
		// from imgui_impl_vulkan.h
		// Initialization data, for ImGui_ImplVulkan_Init()
		// - VkDescriptorPool should be created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
		//   and must contain a pool size large enough to hold an ImGui VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER descriptor.
		// - When using dynamic rendering, set UseDynamicRendering=true and fill PipelineRenderingCreateInfo structure.
		// [Please zero-clear before use!]

		VkDescriptorPoolSize poolSizes[] = {
			// SaplerとImageを組み合わせた分だけプールを確保する
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}
		};

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; //
		createInfo.maxSets = 1;
		createInfo.poolSizeCount = (uint32_t)(IM_ARRAYSIZE(poolSizes));
		createInfo.pPoolSizes = poolSizes;

		VK_CHECK_RESULT(vkCreateDescriptorPool(pVulkanAPI->GetLogicalDevice(), &createInfo, nullptr, &m_ImGuiDescriptorPool));

		return true;
	}
}
#endif