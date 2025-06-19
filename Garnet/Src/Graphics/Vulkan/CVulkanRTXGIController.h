#pragma once

#if defined(USE_RTXGI) && defined(USE_VULKAN)

#include <string>
#include <rtxgi/ddgi/gfx/DDGIVolume_VK.h>

#include "../CRTXGIController.h"
#include "../../Graphics/CTexture.h"

namespace api { class CVulkanAPI; }

namespace graphics
{
	class CVulkanRTXGIController : public CRTXGIController
	{

	private:
		bool CreateTextures(api::IGraphicsAPI* pGraphicsAPI);
		
		bool CreateDDGIVolume(api::IGraphicsAPI* pGraphicsAPI);
		rtxgi::DDGIVolumeDesc CreateDDGIVolumeDesc();
		rtxgi::vulkan::DDGIVolumeResources CreateDDGIVolumeResources(api::IGraphicsAPI* pGraphicsAPI);

	public:
		CVulkanRTXGIController();
		virtual ~CVulkanRTXGIController();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
		bool Draw(api::IGraphicsAPI* pGraphicsAPI) override;

	};
}
#endif // USE_RTXGI && USE_VULKAN