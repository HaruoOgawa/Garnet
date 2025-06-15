#if defined(USE_RTXGI) && defined(USE_VULKAN)
#include "CVulkanRTXGIController.h"

using namespace rtxgi;
using namespace rtxgi::vulkan;

namespace graphics
{
	CVulkanRTXGIController::CVulkanRTXGIController():
		CRTXGIController()
	{
	}

	CVulkanRTXGIController::~CVulkanRTXGIController()
	{
	}

	bool CVulkanRTXGIController::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		DDGIVolumeDesc volumeDesc;

		DDGIVolumeResources volumeResources;

		DDGIVolume* volume = new DDGIVolume();
		ERTXGIStatus status = volume->Create(volumeDesc, volumeResources);

		return true;
	}

	bool CVulkanRTXGIController::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CVulkanRTXGIController::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
}
#endif // USE_RTXGI && USE_VULKAN