#include "CBinaryAnalyser.h"

namespace binary
{
	CBinaryAnalyser::CBinaryAnalyser(const std::vector<unsigned char>& Data):
		m_Data(Data)
	{
		m_Pointer = &m_Data[0];
		m_Offset = 0;
	}

	CBinaryAnalyser::~CBinaryAnalyser()
	{
	}

	unsigned char* CBinaryAnalyser::GetPointer() const
	{
		return m_Pointer;
	}

	bool CBinaryAnalyser::IsValid(size_t ByteSize)
	{
		if (m_Offset + ByteSize >= m_Data.size()) return false;

		return true;
	}

	void CBinaryAnalyser::UpdatePointer(size_t ByteSize)
	{
		m_Offset += ByteSize;

		m_Pointer = &m_Data[m_Offset];
	}

	bool CBinaryAnalyser::Skip(size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		UpdatePointer(ByteSize);

		return true;
	}

	bool CBinaryAnalyser::GetString(std::string& Dst, size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		Dst.resize(ByteSize);
		std::memcpy(&Dst[0], m_Pointer, ByteSize);
		
		UpdatePointer(ByteSize);

		return true;
	}

	/*bool CBinaryAnalyser::GetUTF16String(std::string& Dst, size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		// wstringÇÕUTF16ÅAstringÇÕUTF8
		std::wstring u16;
		u16.resize(ByteSize);
		std::memcpy(&u16[0], m_Pointer, ByteSize);

		// UTF16Ç©ÇÁUTF8Ç…ïœä∑
		// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte/3999597#3999597
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &u16[0], (int)u16.size(), NULL, 0, NULL, NULL);
		Dst.resize(size_needed, 0);

		WideCharToMultiByte(CP_UTF8, 0, &u16[0], (int)u16.size(), &Dst[0], size_needed, NULL, NULL);

		UpdatePointer(ByteSize);

		return true;
	}*/

	bool CBinaryAnalyser::GetInt(int& Dst)
	{
		if (!IsValid(sizeof(int))) return false;

		Dst = GetInt();

		return true;
	}

	int CBinaryAnalyser::GetInt()
	{
		auto val = (m_Pointer[3] << 24) | (m_Pointer[2] << 16) | (m_Pointer[1] << 8) | (m_Pointer[0]);

		int Dst = *reinterpret_cast<const int*>(&val);

		UpdatePointer(sizeof(int));

		return Dst;
	}

	bool CBinaryAnalyser::GetFloat(float& Dst)
	{
		if (!IsValid(sizeof(float))) return false;

		Dst = GetFloat();

		return true;
	}

	float CBinaryAnalyser::GetFloat()
	{
		auto val = (m_Pointer[3] << 24) | (m_Pointer[2] << 16) | (m_Pointer[1] << 8) | (m_Pointer[0]);

		float Dst = *reinterpret_cast<const float*>(&val);

		UpdatePointer(sizeof(float));

		return Dst;
	}

	bool CBinaryAnalyser::GetByte(unsigned char& Dst)
	{
		if (!IsValid(sizeof(unsigned char))) return false;

		Dst = GetByte();

		return true;
	}
	unsigned char CBinaryAnalyser::GetByte()
	{
		unsigned char Dst = m_Pointer[0];

		UpdatePointer(sizeof(unsigned char));

		return Dst;
	}

	bool CBinaryAnalyser::GetUShort(unsigned short& Dst)
	{
		if (!IsValid(sizeof(unsigned short))) return false;

		Dst = GetUShort();

		return true;
	}

	unsigned short CBinaryAnalyser::GetUShort()
	{
		auto val = ((m_Pointer[1] << 8) | (m_Pointer[0]));

		unsigned short Dst = *reinterpret_cast<const unsigned short*>(&val);

		UpdatePointer(sizeof(unsigned short));

		return Dst;
	}
}