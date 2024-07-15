#pragma once

#include "../Math/CMath.h"

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

	class CUniformValueType
	{
	public:
		static math::EValueType CastUniformToValueType(EUniformValueType UniformValueType)
		{
			switch (UniformValueType)
			{
			case graphics::EUniformValueType::NONE:
				return math::EValueType::VALUE_TYPE_NONE;
			case graphics::EUniformValueType::VALUE_TYPE_MAT4:
				return math::EValueType::VALUE_TYPE_MAT4;
			case graphics::EUniformValueType::VALUE_TYPE_MAT3:
				return math::EValueType::VALUE_TYPE_MAT3;
			case graphics::EUniformValueType::VALUE_TYPE_MAT2:
				return math::EValueType::VALUE_TYPE_MAT2;
			case graphics::EUniformValueType::VALUE_TYPE_VEC4:
				return math::EValueType::VALUE_TYPE_VEC4;
			case graphics::EUniformValueType::VALUE_TYPE_VEC3:
				return math::EValueType::VALUE_TYPE_VEC3;
			case graphics::EUniformValueType::VALUE_TYPE_VEC2:
				return math::EValueType::VALUE_TYPE_VEC2;
			case graphics::EUniformValueType::VALUE_TYPE_FLOAT:
				return math::EValueType::VALUE_TYPE_SCALAR;
			case graphics::EUniformValueType::VALUE_TYPE_INT:
				return math::EValueType::VALUE_TYPE_SCALAR;
			case graphics::EUniformValueType::VALUE_TYPE_FLOAT_ARRAY:
				return math::EValueType::VALUE_TYPE_NONE;
			case graphics::EUniformValueType::VALUE_TYPE_MAT4_ARRAY:
				return math::EValueType::VALUE_TYPE_NONE;
			default:
				return math::EValueType::VALUE_TYPE_NONE;
			}

			return math::EValueType::VALUE_TYPE_NONE;
		}
	};
}