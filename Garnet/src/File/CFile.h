#pragma once
#include <vector>
#include <string>

namespace file
{
	class CFile
	{
	public:
		static std::vector<unsigned char> ReadFile(const std::string& filename);
	};
}