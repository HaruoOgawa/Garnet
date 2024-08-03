#pragma once

//#ifdef USE_GUIENGINE

#include "../../Interface/IGraphicsAPI.h"
#include <glm/glm.hpp>

namespace graphics { class CTexture; }

namespace gui
{
	class IImGuiCore
	{
	public:
		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual void Release(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool BeginFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual bool EndFrame(api::IGraphicsAPI* pGraphicsAPI) = 0;

		virtual void* CastTexID(graphics::CTexture* pTexture) = 0;
	};
}
//#endif