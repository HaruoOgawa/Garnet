#pragma once
#include <memory>
#include <vector>
#include <string>
#include "CPresetPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"

namespace renderer {
	class IRenderer;
	class CRendererCreateInfo;
}

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterial;

	class CPrimitive
	{
		std::shared_ptr<renderer::IRenderer>				 m_Renderer;
		std::shared_ptr<renderer::IRenderer>				 m_DepthRenderer;
		std::shared_ptr<renderer::CRendererCreateInfo>		 m_CreateInfo;

		int													 m_MaterialIndex;
	public:
		CPrimitive(const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, int MaterialIndex);
		virtual ~CPrimitive();

		void Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::string& PassName, const std::shared_ptr<graphics::CMaterial>& Material, bool IsDepth);

		bool Draw(const std::shared_ptr<CMaterial>& Material, int DynamicOffsetNum, bool IsDepth);

		int GetMaterialIndex() const;
	};
}