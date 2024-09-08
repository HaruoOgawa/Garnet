#ifdef USE_PNG_PARSER
#include "CPNGParserer.h"
#include <zlib.h>

namespace image_parse
{
	bool CPNGParserer::Parse(const std::vector<unsigned char>& Data, std::vector<unsigned char>& outPixelData, int& Width, int& Height)
	{
		std::vector<uint8_t> compressed_data;

		// zlibを使ってデータを解凍
		std::vector<uint8_t> decompressed_data;  // フィルタバイト込み
		
		uLongf decompressed_size = decompressed_data.size();
		int result = uncompress(decompressed_data.data(), &decompressed_size, compressed_data.data(), compressed_data.size());

		return true;
	}
}
#endif