#pragma once

namespace math
{
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

	};
}