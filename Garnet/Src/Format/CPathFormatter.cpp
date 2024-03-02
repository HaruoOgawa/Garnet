#include "CPathFormatter.h"

namespace format
{
	std::string CPathFormatter::GetExtention(const std::string& Path)
	{
		auto pos = Path.find(".");
		std::string Extention = Path.substr(pos + 1);

		return Extention;
	}

	std::string CPathFormatter::GetParentDir(const std::string& Path)
	{
		auto pos = Path.rfind("\\");
		if (pos == -1) pos = Path.rfind("/");

		if (pos == -1) return Path;

		std::string ParentDir = Path.substr(0, pos);

		return ParentDir;
	}
}