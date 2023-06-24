#pragma once

#include "ETextureType.h"

namespace graphics
{
	struct STextureBindingLayout
	{
		int ViewBindingIndex = -1;
		int SamplerBindingIndex = -1;
		int TextureIndex = -1;
		ETextureType TextureType = ETextureType::TEXTURE_2D;
	};
}