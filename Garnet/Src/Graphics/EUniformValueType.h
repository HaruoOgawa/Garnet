#pragma once

namespace graphics
{
	enum class EUniformValueType
	{
		NONE = -1,
		VALUE_TYPE_MAT4,
		VALUE_TYPE_MAT3,
		VALUE_TYPE_MAT2,
		VALUE_TYPE_VEC4,
		VALUE_TYPE_VEC3,
		VALUE_TYPE_VEC2,
		VALUE_TYPE_FLOAT,
		VALUE_TYPE_INT,
		VALUE_TYPE_FLOAT_ARRAY,
		VALUE_TYPE_MAT4_ARRAY,
	};
}