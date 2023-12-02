#pragma once

#include <string>
#include <vector>
#include "EBufferUpdateTypeh.h"

namespace graphics
{
	struct SBufferValueLayout
	{
		std::string Name = "";
		std::vector<float> Data;
		int ByteSize = 0;
		int BindingIndex = -1;
		EBufferUpdateType BufferUpdateType = EBufferUpdateType::UPDATE_TYPE_CPU;
	};
}