#pragma once

#include <string>
#include <vector>

namespace format
{
	class CStringFormatter
	{
	public:
		static std::vector<std::string> Sprint(std::string Src, char Key);
	};
}