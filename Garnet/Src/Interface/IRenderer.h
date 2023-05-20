#pragma once

#include <memory>

namespace graphics { class CMaterial; }

namespace renderer
{
	class CRendererCreateInfo;

	class IRenderer
	{
	public:
		virtual bool Create(const std::shared_ptr<CRendererCreateInfo>& createInfo, const std::shared_ptr<graphics::CMaterial>& Material) = 0;
		virtual bool Draw(const std::shared_ptr<graphics::CMaterial>& Material, int DynamicOffsetNum) = 0;
	};
}