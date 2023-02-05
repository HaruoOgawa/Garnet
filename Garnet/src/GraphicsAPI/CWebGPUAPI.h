#pragma once
#include "../Interface/IGraphicsAPI.h"

namespace api
{
	class CWebGPUAPI : public IGraphicsAPI
	{
	public:
		CWebGPUAPI();
		virtual ~CWebGPUAPI();

		bool Initialize() override;
	};
}