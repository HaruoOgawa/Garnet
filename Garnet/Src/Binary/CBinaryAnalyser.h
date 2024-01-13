#pragma once

#include <vector>
#include <string>
//#include <Windows.h>

namespace binary
{
	class CBinaryAnalyser
	{
		std::vector<unsigned char> m_Data;
		
		unsigned char* m_Pointer;
		size_t m_Offset;

	private:
		void UpdatePointer(size_t ByteSize);
	public:
		CBinaryAnalyser(const std::vector<unsigned char>& Data);
		virtual ~CBinaryAnalyser();

		unsigned char* GetPointer() const;

		bool IsValid(size_t ByteSize);

		bool Skip(size_t ByteSize);

		bool GetString(std::string& Dst, size_t ByteSize);

		//bool GetUTF16String(std::string& Dst, size_t ByteSize);

		bool GetInt(int& Dst);
		int GetInt();

		bool GetFloat(float& Dst);
		float GetFloat();

		bool GetByte(unsigned char& Dst);
		unsigned char GetByte();

		bool GetUShort(unsigned short& Dst);
		unsigned short GetUShort();
	};
}