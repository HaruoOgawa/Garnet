#pragma once
#include <vector>
#include <string>

namespace file
{
	class CFile
	{
	public:
		static std::vector<char> ReadFile(const std::string& filename);
		static std::string ReadFileAsString(const std::string& filename);
	};
}