#pragma once

#ifdef USE_BINARY_READ

#include <vector>
#include <string>
//#include <Windows.h>

namespace binary
{
	class CBinaryReader
	{
		std::vector<unsigned char> m_Data;
		
		unsigned char* m_Pointer;
		size_t m_Offset;

	private:
		void UpdatePointer(size_t ByteSize);
	public:
		CBinaryReader(const std::vector<unsigned char>& Data);
		virtual ~CBinaryReader();

		unsigned char* GetPointer() const;

		bool IsValid(size_t ByteSize);

		bool IsEnd();

		bool Skip(size_t ByteSize);

		bool GetString(std::string& Dst, size_t ByteSize);
		bool GetStringToZeroByte(std::string& Dst);

		bool GetUTF16String(std::wstring& Dst, size_t ByteSize);
		bool GetUTF16ReverseString(std::wstring& Dst, size_t ByteSize);

		bool GetInt(int& Dst);
		int GetInt();

		bool GetUInt(unsigned int& Dst);
		unsigned int GetUInt();

		bool GetUIntReverse(unsigned int& Dst);
		unsigned int GetUIntReverse();

		bool GetFloat(float& Dst);
		float GetFloat();

		bool GetByte(unsigned char& Dst);
		unsigned char GetByte();

		bool GetUShort(unsigned short& Dst);
		unsigned short GetUShort();
		bool GetUShortReverse(unsigned short& Dst);
		unsigned short GetUShortReverse();
		
		bool GetShort(short& Dst);
		short GetShort();

		bool GetBinary(size_t DstByteOffset, std::vector<unsigned char>& Dst, size_t ByteSize);
	};
}
#endif // USE_BINARY_READ