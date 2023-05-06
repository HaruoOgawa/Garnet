#pragma once
#include <memory>
#include <vector>
#include "CPresetPrimitive.h"

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
		std::shared_ptr<renderer::IRenderer> m_Renderer;
		int							         m_MaterialIndex;
	private:
		bool Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, const std::shared_ptr<CMaterial>& Material);
		bool Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, const std::shared_ptr<CMaterial>& Material);
	public:
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, int MaterialIndex, const std::vector<std::shared_ptr<CMaterial>>& MaterialList);
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex, const std::vector<std::shared_ptr<CMaterial>>& MaterialList);
		virtual ~CPrimitive();

		bool Draw(const std::shared_ptr<CMaterial>& Material);

		int GetMaterialIndex()const { return m_MaterialIndex; }
	};
}