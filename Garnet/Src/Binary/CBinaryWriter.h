#pragma once

#ifdef USE_BINARY_WRITE

#include <vector>
#include <string>

namespace binary
{
	class CBinaryWriter
	{
		std::vector<unsigned char> m_Data;
	public:
		CBinaryWriter();
		virtual ~CBinaryWriter();

		const std::vector<unsigned char>& GetData() const;

		void SetString(const std::string& Src);

		void SetUTF16String(const std::wstring& Src);
		void SetUTF16ReverseString(const std::wstring& Src);

		void SetInt(int Src);

		void SetFloat(float Src);

		void SetByte(unsigned char Src);

		void SetUShort(unsigned short Src);
	};
}
#endif // USE_BINARY_WRITE