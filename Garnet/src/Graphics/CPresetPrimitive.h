#pragma once

namespace vertex {
	class CVertexCreateInfo;
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
		static bool CreateBoard(vertex::CVertexCreateInfo& createInfo);
	};
}