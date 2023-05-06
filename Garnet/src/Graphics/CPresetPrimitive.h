#pragma once

namespace renderer {
	class CRendererCreateInfo;
}

namespace graphics
{
	enum class EPresetPrimitiveType
	{
		BOARD,
		BOX,
		POINT,
		SPHERE,
	};

	class CPresetPrimitive
	{
	public:
		static bool CreateBoard(renderer::CRendererCreateInfo& createInfo);
	};
}