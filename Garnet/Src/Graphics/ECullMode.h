#pragma once

namespace graphics
{
	enum class ECullMode
	{
		NOT_SET = -1,

		CULL_NONE = 0,
		CULL_BACK = 1,
		CULL_FRONT = 2,

		Max = 3,
	};
}