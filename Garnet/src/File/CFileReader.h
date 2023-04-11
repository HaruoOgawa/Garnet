#pragma once
#include <vector>
#include <string>

namespace file
{
	class CFileReader
	{
		bool m_IsDone;
		
		std::vector<char> m_Data;

	public:
		CFileReader();
		virtual ~CFileReader();

		void Release();

		void SetIsDone(bool Done);
		bool IsDone()const;

		void ReadFile(const std::string& filename);


		void SetData(const std::vector<char>& Data);
		const std::vector<char>& GetData() const;
	};
}