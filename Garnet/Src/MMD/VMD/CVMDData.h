#pragma once
#ifdef USE_MMD
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

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