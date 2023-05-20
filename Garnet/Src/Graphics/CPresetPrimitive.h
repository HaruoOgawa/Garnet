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
		static bool CreateBoard(std::shared_ptr<renderer::CRendererCreateInfo>& createInfo);
	};
}