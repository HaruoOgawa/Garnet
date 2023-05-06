#pragma once
#include <memory>
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
		int									 m_MaterialIndex;
	private:
		bool Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo);
		bool Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type);
	public:
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo, int MaterialIndex);
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex);
		virtual ~CPrimitive();

		bool Release();

		bool Update(float SecondsTime);
		bool Draw();
	};
}