#pragma once

#ifdef USE_GUIENGINE

#include "../../Interface/IGraphicsAPI.h"

namespace gui
{
	class IImGuiCore
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;
	};
}
#endif