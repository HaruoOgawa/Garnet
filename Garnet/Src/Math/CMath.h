#pragma once

namespace math
{
	// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2
	unsigned int GetNextPowerOfTwo(unsigned int val)
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
}