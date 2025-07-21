#pragma once
#ifdef USE_NETWORK
// FixtureはDMXの各チャンネルが何に使われるかの設計図

#include <vector>
#include <string>

namespace network
{
	struct SDMXFixture
	{
		std::string DeviceName = std::string();

		// データは全て1バイトチャンネルのまま受け取るとして、その各チャンネルの役割を名称で示す
		std::vector<std::string> ChannelNameList;
	};
}
#endif // USE_NETWORK