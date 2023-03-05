#include "CFile.h"
#include <fstream>
#include "../Debug/Message/Console.h"

namespace file
{
	std::vector<unsigned char> CFile::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Console::Log("failed to open file! / filename: %s\n", filename.c_str());
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		std::vector<unsigned char> OutBuffer(fileSize);
		std::memcpy(&OutBuffer[0], reinterpret_cast<unsigned char*>(buffer.data()), fileSize);

		return OutBuffer;
	}
}