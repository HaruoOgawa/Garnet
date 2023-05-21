#pragma once
#include <vector>
#include <string>

namespace file
{
	class CFileReader
	{
		bool m_IsLoaded;
		
		std::vector<char> m_Data;

	public:
		CFileReader();
		virtual ~CFileReader();

		void Release();

		void SetIsLoaded(bool val);
		bool IsLoaded()const;

		void ReadFile(const std::string& filename);

		void SetData(const std::vector<char>& Data);
		const std::vector<char>& GetData() const;
	};
}