#pragma once

#ifdef USE_GUIENGINE

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

		virtual bool DrawTexture(graphics::CTexture* pTexture, const glm::vec2& Size, const glm::vec2& UV0 = glm::vec2(0.0f, 0.0f), const glm::vec2& UV1 = glm::vec2(1.0f, 1.0f)) = 0;
	};
}
#endif