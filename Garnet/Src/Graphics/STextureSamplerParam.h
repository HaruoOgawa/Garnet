#pragma once

namespace graphics
{
	enum class ETextureFilterMode
	{
		NEAREST = 0,
		LINEAR = 1,
	};

	enum class ETextureWrapMode
	{
		REPEAT = 0,
		MIRRORED_REPEAT = 1,
		CLAMP_TO_EDGE = 2,
		CLAMP_TO_BORDER = 3,
	};

	struct STextureSamplerParam
	{
		ETextureFilterMode FilterMode = ETextureFilterMode::LINEAR;
		ETextureWrapMode WrapMode = ETextureWrapMode::CLAMP_TO_EDGE;
		bool EnabledAnisotropy = true; // 異方性フィルタリング。斜めからテクスチャを見たときに汚く見えないようにするための技術
	};
}