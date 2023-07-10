#pragma once

namespace api
{
	enum class ERenderPassFormat
	{
		NONE = -1,

		COLOR_RENDERPASS = 0,
		DEPTH_RENDERPASS = 1,
		COLOR_FLOAT_RENDERPASS = 2,
		DEPTH_FLOAT_RENDERPASS = 3,
	};
}