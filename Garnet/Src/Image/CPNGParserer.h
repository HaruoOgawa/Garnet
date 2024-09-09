#pragma once

#ifdef USE_PNG_PARSER
#include <vector>

namespace binary { class CBinaryReader; }

namespace image_parse
{
	class CPNGParserer
	{
	private:
		static bool CheckPNGHeader(binary::CBinaryReader& Analyser);
	public:
		static bool Parse(const std::vector<unsigned char>& Data, std::vector<unsigned char>& outPixelData, int& Width, int& Height);
	};
}
#endif