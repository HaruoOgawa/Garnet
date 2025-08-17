#include "CScriptCallback.h"

namespace scriptable
{
#ifdef USE_NETWORK
	void CScriptCallback::OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData)
	{
	}
#endif // USE_NETWORK
}
