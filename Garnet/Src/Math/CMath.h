#pragma once

#include <string>

namespace math
{
	enum class EValueType
	{
		VALUE_TYPE_NONE = -1,

		VALUE_TYPE_SCALAR,
		VALUE_TYPE_VEC2,
		VALUE_TYPE_VEC3,
		VALUE_TYPE_VEC4,
		VALUE_TYPE_MAT2,
		VALUE_TYPE_MAT3,
		VALUE_TYPE_MAT4,
		VALUE_TYPE_VECTOR,
		VALUE_TYPE_MATRIX,
	};

	class CMath
	{
	public:
		// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
		static unsigned int CalcNextPowerOfTwo(unsigned int val)
		{
			unsigned int result = val;

			result--;
			result |= result >> 1;
			result |= result >> 2;
			result |= result >> 4;
			result |= result >> 8;
			result |= result >> 16;
			result++;

			return result;
		}

		static unsigned int CalcClosestPowerOfFour(unsigned int val)
		{
			unsigned int result = val;

			result = CalcNextPowerOfTwo(result);

			while (result % 4 != 0)
			{
				result *= 2;
			}

			return result;
		}

		static int GetByteSizeFromValueType(math::EValueType Type)
		{
			if (Type == math::EValueType::VALUE_TYPE_SCALAR)
			{
				return 1 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC2)
			{
				return 2 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC3)
			{
				return 3 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC4)
			{
				return 4 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT2)
			{
				return 4 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT3)
			{
				return 9 * sizeof(float);
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT4)
			{
				return 16 * sizeof(float);
			}
			else
			{
				// Unknown component type
				return 0;
			}
		}

		static int GetNumComponentsInType(math::EValueType Type)
		{
			if (Type == math::EValueType::VALUE_TYPE_SCALAR)
			{
				return 1;
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC2)
			{
				return 2;
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC3)
			{
				return 3;
			}
			else if (Type == math::EValueType::VALUE_TYPE_VEC4)
			{
				return 4;
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT2)
			{
				return 4;
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT3)
			{
				return 9;
			}
			else if (Type == math::EValueType::VALUE_TYPE_MAT4)
			{
				return 16;
			}
			else
			{
				// Unknown component type
				return -1;
			}
		}

		static std::string GetFloatWithPrecision(float value, int precision)
		{
			//
			char format[32];
			snprintf(format, sizeof(format), "%%.%df", precision);

			//
			char buffer[64];
			snprintf(buffer, sizeof(buffer), format, value);

			std::string dst = std::string(buffer);

			return dst;
		}
	};
}