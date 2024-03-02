#pragma once

#include <string>

namespace format
{
	class CPathFormatter
	{
	public:
		static std::string GetExtention(const std::string& Path);

		static std::string GetParentDir(const std::string& Path);
	};
}