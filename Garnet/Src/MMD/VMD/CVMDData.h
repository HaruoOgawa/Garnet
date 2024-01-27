#pragma once
#ifdef USE_MMD
#include <vector>

namespace mmd
{
	class CVMDData
	{
	public:
		CVMDData();
		virtual ~CVMDData();

		bool Analyse(const std::vector<unsigned char>& Data);
	};
}
#endif