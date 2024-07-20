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
	};
}
#endif // USE_BINARY_WRITE