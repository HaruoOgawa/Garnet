#pragma once

#include <string>
#include "ETextureType.h"

namespace graphics
{
	struct STextureBindingLayout
	{
		std::string TextureName = "";
		int ViewBindingIndex = -1;
		int SamplerBindingIndex = -1;
		int TextureIndex = -1;
		ETextureType TextureType = ETextureType::TEXTURE_2D;
	};
}