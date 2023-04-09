#pragma once

#include "IGraphicsAPI.h"

namespace app
{
	class IApp
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool Release(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool ProcessInput(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, float SecondsTime) = 0;
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI) = 0;
	};
}