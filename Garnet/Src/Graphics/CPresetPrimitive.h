#pragma once
#include <memory>

namespace renderer {
	class CRendererCreateInfo;
}

namespace graphics
{
	enum class EPresetPrimitiveType
	{
		None = -1,

		BOARD,
		BOX,
		POINT,
		SPHERE,
	};

	class CPresetPrimitive
	{
	public:
		static std::shared_ptr<renderer::CRendererCreateInfo> CreateBoard();
		static std::shared_ptr<renderer::CRendererCreateInfo> CreateBox();
		static std::shared_ptr<renderer::CRendererCreateInfo> CreatePoint();
		static std::shared_ptr<renderer::CRendererCreateInfo> CreateSphere();
	};
}