#pragma once

#if defined(USE_RTXGI) && defined(USE_VULKAN)

#include <string>
#include "../CRTXGIController.h"
#include <rtxgi/ddgi/gfx/DDGIVolume_VK.h>

namespace graphics
{
	class CVulkanRTXGIController : public CRTXGIController
	{

	private:
		bool CreateDDGIVolume(api::IGraphicsAPI* pGraphicsAPI);

	public:
		CVulkanRTXGIController();
		virtual ~CVulkanRTXGIController();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;

	};
}
#endif // USE_RTXGI && USE_VULKAN