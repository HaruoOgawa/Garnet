#pragma once
namespace api { class IGraphicsAPI; }

namespace renderer
{
	class CRendererCreateInfo;

	class IRenderer
	{
	public:
		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI, const CRendererCreateInfo& createInfo) = 0;
		virtual bool Update(float SecondsTime) = 0;
		virtual bool Draw() = 0;
	};
}