#include "CFileReader.h"
#include <fstream>
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace file
{
	CFileReader::CFileReader():
		m_IsDone(false),
		m_pData(nullptr),
		m_BytesLength(0)
	{
	}

	CFileReader::~CFileReader()
	{
	}

	void CFileReader::Release()
	{
		m_IsDone = false;
		m_Data.clear();

		if (m_pData)
		{
			delete m_pData;
		}

		m_BytesLength = 0;
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
				console.log("failed to load file! / filename: %s", filename);
				HEAP32[$1 >> 2] = 0;
			})
			.then((val) => {
				// ロードしたバッファのメモリをコピーする
				const buffer = val;
				const view = new Int8Array(buffer);
				const stride = Module.HEAP8.BYTES_PER_ELEMENT;

				const resultPtr = Module._malloc(buffer.byteLength);
				Module.HEAP8.set(view, resultPtr / stride);
				HEAP8[$2 >> 2] = resultPtr;
				HEAP32[$3 >> 2] = buffer.byteLength;
				//Module._free(resultPtr);

				console.log("val: %o", val);
				console.log("view: %o", view);
				console.log("stride: %o", stride);
				console.log("resultPtr: %o", resultPtr);

				HEAP32[$1 >> 2] = 1;
			}, () => {
				console.log("failed to load file! / filename: %s", filename);
				HEAP32[$1 >> 2] = 0;
			})
		}, filename.c_str(), &m_IsDone, m_pData, &m_BytesLength);
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

	bool CFileReader::BuildData()
	{
		Console::Log("m_pData: %p / m_BytesLength: %d\n", m_pData, static_cast<int>(m_BytesLength));

		if (!m_pData || m_BytesLength <= 0) return false;

		return true;
	}


	const std::vector<char>& CFileReader::GetData() const
	{
		return m_Data;
	}
}