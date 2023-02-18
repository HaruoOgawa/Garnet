#pragma once

#include "IGraphicsAPI.h"

namespace app
{
	class IApp
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool ProcessInput() = 0;
		virtual bool Update() = 0;
		virtual bool Draw() = 0;
	};
}