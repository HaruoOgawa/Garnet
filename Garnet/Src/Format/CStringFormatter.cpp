#include "CStringFormatter.h"

namespace format
{
	std::vector<std::string> CStringFormatter::Sprit(std::string Src, char Key)
	{
		std::vector<std::string> ParamList;

		if (!Src.empty())
		{
			std::string currentStr = std::string();

			for (int i = 0; i < static_cast<int>(Src.size()); i++)
			{
				char c = Src.data()[i];

				if (c == Key)
				{
					if(!currentStr.empty()) ParamList.push_back(currentStr);
					currentStr = std::string();

					continue;
				}

				currentStr += c;
			}

			ParamList.push_back(currentStr);
			currentStr = std::string();
		}

		return ParamList;
	}
}