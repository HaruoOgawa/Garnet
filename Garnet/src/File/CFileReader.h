#pragma once
#include <vector>
#include <string>

namespace file
{
	class CFileReader
	{
		bool m_IsDone;
		
		std::vector<char> m_Data;
		char* m_pData;
		size_t m_BytesLength;

	public:
		CFileReader();
		virtual ~CFileReader();

		void Release();

		bool IsDone()const;

		void ReadFile(const std::string& filename);

		bool BuildData();
		const std::vector<char>& GetData() const;
	};
}