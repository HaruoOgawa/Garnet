#pragma once
#include <memory>
#include <vector>
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
		int													 m_MaterialIndex;
		int													 m_DynamicOffsetNum;

		const EPresetPrimitiveType							 m_PresetType;
		std::shared_ptr<renderer::CRendererCreateInfo>		 m_CreateInfo;
	private:
		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material, const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo);
		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& Material, EPresetPrimitiveType PresetType);
	public:
		CPrimitive(int MaterialIndex, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList, const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo, 
			EPresetPrimitiveType PresetType = EPresetPrimitiveType::None);
		virtual ~CPrimitive();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::shared_ptr<graphics::CMaterial>>& MaterialList);

		bool Draw(const std::shared_ptr<CMaterial>& Material);

		int GetMaterialIndex()const { return m_MaterialIndex; }
	};
}