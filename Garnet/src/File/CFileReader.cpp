#include "CFileReader.h"
#include <fstream>
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace file
{
	CFileReader::CFileReader():
		m_IsDone(false)
	{
	}

	CFileReader::~CFileReader()
	{
	}

	void CFileReader::Release()
	{
		m_IsDone = false;
		m_Data.clear();
	}

	bool CFileReader::IsDone()const
	{
		return m_IsDone;
	}

	void CFileReader::ReadFile(const std::string& filename)
	{
		std::string result = "";
#ifdef __EMSCRIPTEN__
		EM_ASM({
			const filename = Module.UTF8ToString($0);

			fetch(filename)
			.then((res) => {
				return res.arrayBuffer();
			}, () => {
				console.log("load failure");
				HEAP32[$1 >> 2] = 0;
			})
			.then((val) => {
				// ロードしたバッファのメモリをコピーする
				console.log("val: %o", val);

				HEAP32[$1 >> 2] = 1;
			}, () => {
				console.log("load failure");
				HEAP32[$1 >> 2] = 0;
			})
		}, filename.c_str(), &m_IsDone, &m_Data[0]);
#else
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Console::Log("failed to open file! / filename: %s\n", filename.c_str());
		}

		size_t fileSize = (size_t)file.tellg();
		m_Data.resize(fileSize);

		file.seekg(0);
		file.read(m_Data.data(), fileSize);

		file.close();

		m_IsDone = true;
#endif
	}
}