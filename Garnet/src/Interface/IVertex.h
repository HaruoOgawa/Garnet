#pragma once

namespace api { class IGraphicsAPI; }

namespace vertex
{
	class CVertexCreateInfo;

	class IVertex
	{
	public:
		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI, const CVertexCreateInfo& createInfo) = 0;
	};
}