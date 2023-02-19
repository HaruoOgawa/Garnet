#pragma once
#include <memory>

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
		std::shared_ptr<CMaterial> m_Material;
	public:
		CPrimitive();
		virtual ~CPrimitive();

		bool Release();

		bool Create(api::IGraphicsAPI* pGraphicsAPI, const renderer::CRendererCreateInfo& createInfo);

		bool Update();
		bool Draw();
	};
}