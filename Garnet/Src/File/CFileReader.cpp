#include "CFileReader.h"
#include <fstream>
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#endif

namespace file
{
	CFileReader::CFileReader():
		m_IsLoaded(false)
	{
	}

	CFileReader::~CFileReader()
	{
	}

	void CFileReader::Release()
	{
		m_IsLoaded = false;
		m_Data.clear();
	}

	void CFileReader::SetIsLoaded(bool val)
	{
		m_IsLoaded = val;
	}

	bool CFileReader::IsLoaded()const
	{
		return m_IsLoaded;
	}

#ifdef __EMSCRIPTEN__
	void downloadSucceded(emscripten_fetch_t* fetch)
	{
		std::vector<unsigned char> Data;
		Data.resize(fetch->numBytes);

		std::memcpy(&Data[0], reinterpret_cast<const unsigned char*>(fetch->data), fetch->numBytes);

		auto fileReader = static_cast<CFileReader*>(fetch->userData);
		fileReader->SetData(Data);
		fileReader->SetIsLoaded(true);

		emscripten_fetch_close(fetch);
	}

	void downloadFailed(emscripten_fetch_t* fetch)
	{
		Console::Log("[downloadFailed] fetch->url: %s\n", fetch->url);
		emscripten_fetch_close(fetch);
	}
#endif

	void CFileReader::ReadFile(const std::string& filename)
	{
		std::string result = "";
#ifdef __EMSCRIPTEN__
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		std::strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		attr.onsuccess = downloadSucceded;
		attr.onerror = downloadFailed;
		attr.userData = this;
		emscripten_fetch(&attr, filename.c_str());

#else
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Console::Log("failed to open file! / filename: %s\n", filename.c_str());
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> ReadData;
		ReadData.resize(fileSize);

		file.seekg(0);
		file.read(ReadData.data(), fileSize);

		file.close();

		//
		m_Data.resize(fileSize);
		std::memcpy(&m_Data[0], reinterpret_cast<const unsigned char*>(&ReadData[0]), fileSize);

		m_IsLoaded = true;
#endif
	}

	void CFileReader::SetData(const std::vector<unsigned char>& Data)
	{
		m_Data = Data;
	}

	const std::vector<unsigned char>& CFileReader::GetData() const
	{
		return m_Data;
	}
}