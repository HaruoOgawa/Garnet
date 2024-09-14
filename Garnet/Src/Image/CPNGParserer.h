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
		static bool UnfilterScanline(std::vector<unsigned char>& Recon, const std::vector<unsigned char>& Scanline, const std::vector<unsigned char>& PrevScanline, unsigned char FilterType, int BPP);
		
		static int paethPredictor(int a, int b, int c);
	public:
		static bool Parse(const std::vector<unsigned char>& Data, std::vector<unsigned char>& outPixelData, int& Width, int& Height);
	};
}
#endif