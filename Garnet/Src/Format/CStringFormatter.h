#pragma once

#include <string>
#include <vector>

namespace format
{
	class CStringFormatter
	{
	public:
		static std::vector<std::string> Sprit(std::string Src, char Key);
	};
}