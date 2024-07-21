#ifdef USE_BINARY_WRITE
#include "CBinaryWriter.h"

namespace binary
{
	CBinaryWriter::CBinaryWriter()
	{
	}

	CBinaryWriter::~CBinaryWriter()
	{
	}

	const std::vector<unsigned char>& CBinaryWriter::GetData() const
	{
		return m_Data;
	}

	void CBinaryWriter::SetString(const std::string& Src)
	{
		std::vector<unsigned char> val(Src.begin(), Src.end());
		int SrcByteSize = static_cast<int>(val.size());

		int ByteSize = static_cast<int>(m_Data.size());
		m_Data.resize(ByteSize + SrcByteSize);

		std::memcpy(&m_Data[ByteSize], &val[0], SrcByteSize);
	}

	void CBinaryWriter::SetUTF16String(const std::wstring& Src)
	{
		std::vector<wchar_t> val(Src.begin(), Src.end());
		int SrcByteSize = static_cast<int>(val.size());
		
		int ByteSize = static_cast<int>(m_Data.size());
		m_Data.resize(ByteSize + SrcByteSize);

		std::memcpy(&m_Data[ByteSize], &val[0], SrcByteSize);
	}

	void CBinaryWriter::SetUTF16ReverseString(const std::wstring& Src)
	{
		std::vector<wchar_t> val(Src.begin(), Src.end());
		
		// 順番を逆にする
		std::reverse(val.begin(), val.end());

		int SrcByteSize = static_cast<int>(val.size());

		int ByteSize = static_cast<int>(m_Data.size());
		m_Data.resize(ByteSize + SrcByteSize);

		std::memcpy(&m_Data[ByteSize], &val[0], SrcByteSize);
	}

	void CBinaryWriter::SetInt(int Src)
	{
		const unsigned int val = *reinterpret_cast<const unsigned int*>(&Src);

		// 0xffのアンド演算(両方のビットが1の時だけ1にする)することで、8ビット目よりも先のビットを0にして削除する
		m_Data.push_back(static_cast<unsigned char>(val & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 8) & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 16) & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 24) & 0xff));
	}

	void CBinaryWriter::SetFloat(float Src)
	{
		const unsigned int val = *reinterpret_cast<const unsigned int*>(&Src);

		// 0xffのアンド演算(両方のビットが1の時だけ1にする)することで、8ビット目よりも先のビットを0にして削除する
		m_Data.push_back(static_cast<unsigned char>(val & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 8) & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 16) & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 24) & 0xff));
	}

	void CBinaryWriter::SetByte(unsigned char Src)
	{
		m_Data.push_back(Src);
	}

	void CBinaryWriter::SetUShort(unsigned short Src)
	{
		// 0xffのアンド演算(両方のビットが1の時だけ1にする)することで、8ビット目よりも先のビットを0にして削除する
		const unsigned int val = *reinterpret_cast<const unsigned int*>(&Src);

		// 0xffのアンド演算(両方のビットが1の時だけ1にする)することで、8ビット目よりも先のビットを0にして削除する
		m_Data.push_back(static_cast<unsigned char>(val & 0xff));
		m_Data.push_back(static_cast<unsigned char>((val >> 8) & 0xff));
	}
}
#endif // USE_BINARY_WRITE