#ifdef USE_BINARY_READ

#include "CBinaryReader.h"

namespace binary
{
	CBinaryReader::CBinaryReader(const std::vector<unsigned char>& Data):
		m_Data(Data)
	{
		m_Pointer = &m_Data[0];
		m_Offset = 0;
	}

	CBinaryReader::~CBinaryReader()
	{
	}

	unsigned char* CBinaryReader::GetPointer() const
	{
		return m_Pointer;
	}

	bool CBinaryReader::IsValid(size_t ByteSize)
	{
		if (m_Offset + ByteSize > m_Data.size()) return false;

		return true;
	}

	bool CBinaryReader::IsEnd()
	{
		return (m_Data.size() == m_Offset);
	}

	void CBinaryReader::UpdatePointer(size_t ByteSize)
	{
		m_Offset += ByteSize;

		// 最後まで読み取っていたらこれ以上は更新しない
		if (IsEnd()) return;

		m_Pointer = &m_Data[m_Offset];
	}

	bool CBinaryReader::Skip(size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		UpdatePointer(ByteSize);

		return true;
	}

	bool CBinaryReader::GetString(std::string& Dst, size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		Dst.resize(ByteSize);
		std::memcpy(&Dst[0], m_Pointer, ByteSize);
		
		UpdatePointer(ByteSize);

		return true;
	}

	bool CBinaryReader::GetUTF16String(std::wstring& Dst, size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		// wstringはUTF16、stringはUTF8
		Dst.resize(ByteSize);
		std::memcpy(&Dst[0], m_Pointer, ByteSize);

		UpdatePointer(ByteSize);

		return true;
	}
	
	bool CBinaryReader::GetUTF16ReverseString(std::wstring& Dst, size_t ByteSize)
	{
		if (!IsValid(ByteSize)) return false;

		std::vector<unsigned char> Data;
		for (int i = 0; i < ByteSize; i += 2)
		{
			Data.push_back(m_Pointer[i + 1]);
			Data.push_back(m_Pointer[i]);
		}

		if (ByteSize % 2 != 0)
		{
			Data.push_back(m_Pointer[ByteSize - 1]);
		}

		// wstringはUTF16、stringはUTF8
		Dst.resize(ByteSize);
		std::memcpy(&Dst[0], &Data[0], ByteSize);

		UpdatePointer(ByteSize);

		return true;
	}

	bool CBinaryReader::GetInt(int& Dst)
	{
		if (!IsValid(sizeof(int))) return false;

		Dst = GetInt();

		return true;
	}

	int CBinaryReader::GetInt()
	{
		auto val = (m_Pointer[3] << 24) | (m_Pointer[2] << 16) | (m_Pointer[1] << 8) | (m_Pointer[0]);

		int Dst = *reinterpret_cast<const int*>(&val);

		UpdatePointer(sizeof(int));

		return Dst;
	}

	bool CBinaryReader::GetFloat(float& Dst)
	{
		if (!IsValid(sizeof(float))) return false;

		Dst = GetFloat();

		return true;
	}

	float CBinaryReader::GetFloat()
	{
		auto val = (m_Pointer[3] << 24) | (m_Pointer[2] << 16) | (m_Pointer[1] << 8) | (m_Pointer[0]);

		float Dst = *reinterpret_cast<const float*>(&val);

		UpdatePointer(sizeof(float));

		return Dst;
	}

	bool CBinaryReader::GetByte(unsigned char& Dst)
	{
		if (!IsValid(sizeof(unsigned char))) return false;

		Dst = GetByte();

		return true;
	}
	unsigned char CBinaryReader::GetByte()
	{
		unsigned char Dst = m_Pointer[0];

		UpdatePointer(sizeof(unsigned char));

		return Dst;
	}

	bool CBinaryReader::GetUShort(unsigned short& Dst)
	{
		if (!IsValid(sizeof(unsigned short))) return false;

		Dst = GetUShort();

		return true;
	}

	unsigned short CBinaryReader::GetUShort()
	{
		auto val = ((m_Pointer[1] << 8) | (m_Pointer[0]));

		unsigned short Dst = *reinterpret_cast<const unsigned short*>(&val);

		UpdatePointer(sizeof(unsigned short));

		return Dst;
	}
}

#endif // USE_BINARY_READ