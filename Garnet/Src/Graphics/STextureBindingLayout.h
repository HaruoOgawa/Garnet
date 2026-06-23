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
		bool ReadOnFragment = true;
		bool ReadOnVertex = false;
		std::string FrameName = std::string();
		int FrameIndex = -1;

	public:
		STextureBindingLayout(std::string _TextureName, int _ViewBindingIndex, int _SamplerBindingIndex, 
			int _TextureIndex, ETextureUsage _TextureUsage,
			bool _ReadOnFragment, bool _ReadOnVertex, std::string _FrameName, int _FrameIndex):
			TextureName(_TextureName),
			ViewBindingIndex(_ViewBindingIndex),
			SamplerBindingIndex(_SamplerBindingIndex),
			TextureIndex(_TextureIndex),
			TextureUsage(_TextureUsage),
			ReadOnFragment(_ReadOnFragment),
			ReadOnVertex(_ReadOnVertex),
			FrameName(_FrameName),
			FrameIndex(_FrameIndex)
		{
		}
	};
}