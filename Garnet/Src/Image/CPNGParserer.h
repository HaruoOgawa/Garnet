#pragma once

#ifdef USE_PNG_PARSER
#include <vector>

namespace image_parse
{
	class CPNGParserer
	{
	private:

	public:
		static bool Parse(const std::vector<unsigned char>& Data, std::vector<unsigned char>& outPixelData, int& Width, int& Height);
	};
}
#endif