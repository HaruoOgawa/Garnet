#pragma once

namespace api
{
	// ToDo:この列挙型はレンダーパス以外のテクスチャフォーマット指定にも使っているので名前をリファクタリングしたい
	enum class ERenderPassFormat
	{
		NONE = -1,

		COLOR_RENDERPASS,
		COLOR_FLOAT_RENDERPASS,
		DEPTH_RENDERPASS,
		DEPTH_FLOAT_RENDERPASS,
		DEPTH_STENCIL_RENDERPASS,
		DEPTH_STENCIL_FLOAT_RENDERPASS,
		COLOR_BGRA,
		COLOR_USHORT_RENDERPASS,
	};
}