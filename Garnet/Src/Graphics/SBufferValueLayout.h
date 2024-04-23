#pragma once

#include <string>
#include <vector>

#include "EUniformValueType.h"
#include "SUniformValueInput.h"

namespace graphics
{
	struct SBufferValueLayout
	{
		std::string Name = "";
		EUniformValueType ValueType = EUniformValueType::NONE;
		std::vector<float> Data;
		int ByteSize = 0;
		int BindingIndex = -1;
		SUniformValueInput ValueInput = {};
	};
}