#pragma once

#include <vector>
#include <memory>
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace renderer {
	class IRenderer;
	class CRendererCreateInfo;
}

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CUniformBufferDescriptor;

	class CMaterial
	{
		std::shared_ptr<renderer::IRenderer> m_Renderer;
		std::vector<CUniformBufferDescriptor> m_UBODescList;
	public:
		CMaterial();
		virtual ~CMaterial();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo);
	};
}