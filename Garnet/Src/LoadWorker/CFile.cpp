#include "CFile.h"
#include <fstream>
#include "../Debug/Message/Console.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#endif

namespace resource
{
	CFile::CFile(const std::string& filename):
		m_Status(resource::ELoadStatus::None),
		m_Filename(filename),
		m_U16Filename(L""),
		m_U16Extention(L""),
		m_IsSync(false)
	{
		auto pos = m_Filename.find(".");
		m_Extention = m_Filename.substr(pos + 1);
	}
	
	CFile::CFile(const std::wstring& filename):
		m_Status(resource::ELoadStatus::None),
		m_Filename(""),
		m_Extention(""),
		m_U16Filename(filename),
		m_IsSync(false)
	{
		auto pos = m_U16Filename.find(L".");
		m_U16Extention = m_U16Filename.substr(pos + 1);
	}

	CFile::~CFile()
	{
	}

	const std::string& CFile::GetExtention() const
	{
		return m_Extention;
	}

	void CFile::SetIsSync(bool IsSync)
	{
		m_IsSync = IsSync;
	}

	void CFile::Release()
	{
		m_Status = resource::ELoadStatus::None;
		m_Data.clear();
	}

	void CFile::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus CFile::GetStatus() const
	{
		return m_Status;
	}

	bool CFile::IsLoaded()const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

#ifdef __EMSCRIPTEN__
	void downloadSucceded(emscripten_fetch_t* fetch)
	{
		std::vector<unsigned char> Data;
		Data.resize(fetch->numBytes);

		std::memcpy(&Data[0], reinterpret_cast<const unsigned char*>(fetch->data), fetch->numBytes);

		auto fileReader = static_cast<CFile*>(fetch->userData);
		fileReader->SetData(Data);
		fileReader->SetLoadStatus(resource::ELoadStatus::Loaded);

		emscripten_fetch_close(fetch);
	}

	void downloadFailed(emscripten_fetch_t* fetch)
	{
		Console::Log("[downloadFailed] fetch->url: %s\n", fetch->url);
		emscripten_fetch_close(fetch);
	}
#endif

	bool CFile::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		std::string result = "";
#ifdef __EMSCRIPTEN__
		emscripten_fetch_attr_t attr;
		emscripten_fetch_attr_init(&attr);
		std::strcpy(attr.requestMethod, "GET");
		attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
		if (m_IsSync) attr.attributes |= EMSCRIPTEN_FETCH_SYNCHRONOUS;
		attr.onsuccess = downloadSucceded;
		attr.onerror = downloadFailed;
		attr.userData = this;
		emscripten_fetch(&attr, m_Filename.c_str());

#else
		std::ifstream file;

		if (!m_Filename.empty())
		{
			file = std::ifstream(m_Filename, std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				Console::Log("failed to open file! / m_Filename: %s\n", m_Filename.c_str());
			}
		}
		else if (!m_U16Filename.empty())
		{
			file = std::ifstream(m_U16Filename, std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				Console::Log("failed to open file! / m_U16Filename: %ls\n", m_U16Filename.c_str());
			}
		}
		else
		{
			return false;
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

		m_Status = resource::ELoadStatus::Loaded;
#endif

		return true;
	}

	bool CFile::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	void CFile::SetData(const std::vector<unsigned char>& Data)
	{
		m_Data = Data;
	}

	const std::vector<unsigned char>& CFile::GetData() const
	{
		return m_Data;
	}

	std::string CFile::GetParentDir(const std::string& Path)
	{
		if (Path.rfind("\\") != -1)
		{
			return Path.substr(0, Path.rfind("\\"));
		}
		else if (Path.rfind("/") != -1)
		{
			return Path.substr(0, Path.rfind("/"));
		}
		else
		{
			// カレントディレクトリを返す
			return "";
		}
	}

	std::string CFile::AddPunct(const std::string& Path)
	{
		if (Path[Path.length() - 1] == '\\' || Path[Path.length() - 1] == '/')
		{
			return Path;
		}
		else
		{
			return Path + "\\";
		}
	}

	std::wstring CFile::CastU8ToU16Str(const std::string& U8Str)
	{
		std::wstring DstU16Str = std::wstring();

		for (auto c_u8 = U8Str.begin(); c_u8 != U8Str.end(); c_u8++)
		{
			DstU16Str.push_back(static_cast<WCHAR>(*c_u8));
		}

		return DstU16Str;
	}
}