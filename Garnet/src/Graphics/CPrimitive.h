#pragma once
#include <memory>
#include "CPresetPrimitive.h"

namespace vertex {
	class IVertex;
	class CVertexCreateInfo;
}

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterial;

	class CPrimitive
	{
		std::shared_ptr<vertex::IVertex> m_Vertex;
		int							     m_MaterialIndex;
	private:
		bool Create(api::IGraphicsAPI* pGraphicsAPI, const vertex::CVertexCreateInfo& createInfo);
		bool Create(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type);
	public:
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, const vertex::CVertexCreateInfo& createInfo, int MaterialIndex);
		CPrimitive(api::IGraphicsAPI* pGraphicsAPI, EPresetPrimitiveType Type, int MaterialIndex);
		virtual ~CPrimitive();
	};
}