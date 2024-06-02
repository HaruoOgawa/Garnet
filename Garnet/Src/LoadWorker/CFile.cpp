#include "CFile.h"
#include "../Message/Console.h"
#include "../Format/CPathFormatter.h"

namespace resource
{
	CFile::CFile(const std::string& filename):
#ifndef __EMSCRIPTEN__
		m_ByteOffset(0),
#endif // !__EMSCRIPTEN__
		m_Status(resource::ELoadStatus::None),
		m_Filename(filename),
		m_IsSync(false)
	{
		m_Extention = format::CPathFormatter::GetExtention(filename);
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

	const std::string& CFile::GetFilename() const
	{
		return m_Filename;
	}

	int CFile::GetLoadPriority() const
	{
		return 0;
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
		// 大容量ファイルのロード準備だけ. LoadShaderにもアニメーションを付けて止まっていないようにしたい
		if (!LoadImmediate()) return false;
		return true;

		/*m_Status = resource::ELoadStatus::Loading;

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
		m_Stream = std::ifstream(m_Filename, std::ios::ate | std::ios::binary);

		if (!m_Stream.is_open())
		{
			Console::Log("failed to open file! / m_Filename: %s\n", m_Filename.c_str());
			return false;
		}

		size_t fileSize = (size_t)m_Stream.tellg();
		m_Data.resize(fileSize);

		m_Stream.seekg(0);
#endif

		return true;*/
	}

	bool CFile::LoadImmediate()
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
		std::ifstream file = std::ifstream(m_Filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Console::Log("failed to open file! / m_Filename: %s\n", m_Filename.c_str());
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

	bool CFile::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
#ifndef __EMSCRIPTEN__
		if (m_Status == resource::ELoadStatus::Loading)
		{
			// ファイルを一行ずつ読む
			std::string line;
			
			if (m_Stream.fail())
			{
				Console::Log("[Error] Failed to read line\n");

				return false;
			}

			int Loop = 0;
			while (Loop < 128)
			{
				if (getline(m_Stream, line))
				{
					int ByteSize = sizeof(char) * static_cast<int>(line.size());
					std::memcpy(&m_Data[m_ByteOffset], reinterpret_cast<const unsigned char*>(line.data()), ByteSize);

					m_ByteOffset += ByteSize;
				}
				else
				{
					// ロード終了
					m_Stream.close();
					m_Status = resource::ELoadStatus::Loaded;

					break;
				}

				Loop++;
			}
			
		}
#endif // !__EMSCRIPTEN__

		return true;
	}

	void CFile::Reset()
	{
		m_Status = ELoadStatus::None;

		m_Data.clear();
	}

	bool CFile::Reload(resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	void CFile::AddReference(const std::shared_ptr<IResource>& Resource)
	{
		// 自身の時は追加しない
		if (Resource == shared_from_this())
		{
			return;
		}
	}

	void CFile::SetData(const std::vector<unsigned char>& Data)
	{
		m_Data = Data;
	}

	const std::vector<unsigned char>& CFile::GetData() const
	{
		return m_Data;
	}

	bool CFile::Write()
	{
#ifdef __EMSCRIPTEN__
		return true;
#else
		int fileSize = static_cast<int>(m_Data.size());

		std::vector<char> OutputData;
		OutputData.resize(fileSize);
		std::memcpy(&OutputData[0], reinterpret_cast<const char*>(&m_Data[0]), fileSize);

		std::ofstream file;
		file.open(m_Filename);
		file.write(&OutputData[0], fileSize);
		file.close();

		return true;
#endif // __EMSCRIPTEN__

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
			DstU16Str.push_back(static_cast<wchar_t>(*c_u8));
		}

		return DstU16Str;
	}

	std::string CFile::CastU16ToU8Str(const std::wstring& U16Str)
	{
		std::string DstU8Str = std::string();

		for(int i = 0; i < U16Str.length(); i++)
		{
			auto c_u16 = U16Str.data()[i];

#ifdef __EMSCRIPTEN__
			// Web上だとwstringのwchar_tは4バイト・4文字で構成されて偶数番目に実際に使用したい文字が入っているので以下の様に変換する
			std::vector<char> Data;
			Data.resize(4);

			std::memcpy(&Data[0], &c_u16, 4);
			
			DstU8Str.push_back(Data[0]);
			DstU8Str.push_back(Data[2]);
#else
			DstU8Str.push_back(static_cast<char>(c_u16));
#endif
		}

		return DstU8Str;
	}
}