#pragma once

#include <string>
#include "ETextureUsage.h"

namespace graphics
{
	struct STextureBindingLayout
	{
		std::string TextureName = "";
		int ViewBindingIndex = -1;
		int SamplerBindingIndex = -1;
		int TextureIndex = -1;
		ETextureUsage TextureUsage = ETextureUsage::TEXTURE_USAGE_2D;
	};
}