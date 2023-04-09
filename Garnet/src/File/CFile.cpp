#include "CFile.h"
#include <fstream>
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace file
{
	std::vector<char> CFile::ReadFile(const std::string& filename)
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

		return buffer;
	}
	
	std::string CFile::ReadFileAsString(const std::string& filename)
	{
		std::string result = "";
#ifdef __EMSCRIPTEN__
		EM_ASM({
			const filename = Module.UTF8ToString($0);
			console.log("filename: %s", filename);
			fetch(filename)
			.then((res) => {
				return res.text();
			})
			.then((val) => {
				var result = val;
				console.log("result: %s", result);
			})
		}, filename.c_str());
#else
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

		result = std::string(&buffer[0], buffer.size());
#endif
		return result;
	}
}