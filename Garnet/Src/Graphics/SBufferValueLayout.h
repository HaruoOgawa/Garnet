#pragma once

#include <string>
#include <vector>

namespace graphics
{
	struct SBufferValueLayout
	{
		std::string Name = "";
		std::vector<float> Data;
		int ByteSize = 0;
		int BindingIndex = -1;
	};
}