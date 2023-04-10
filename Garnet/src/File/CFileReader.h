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

		bool IsDone()const;

		void ReadFile(const std::string& filename);
	};
}