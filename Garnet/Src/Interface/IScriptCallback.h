#pragma once

#ifdef USE_NETWORK

#include <vector>

namespace network{ struct SDMXFixture; }

class IScriptCallback
{
public:
	virtual bool OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData) = 0;
};

#endif // USE_NETWORK