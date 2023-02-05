#pragma once
#include "../Interface/IGraphicsAPI.h"

namespace api
{
	class CVulkanAPI : public IGraphicsAPI
	{
	public:
		CVulkanAPI();
		virtual ~CVulkanAPI();

		bool Initialize() override;
	};
}