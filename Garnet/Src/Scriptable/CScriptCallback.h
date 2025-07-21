#pragma once

#include <vector>

#ifdef USE_NETWORK
#include "../Network/DMX/SDMXFixture.h"
#endif // USE_NETWORK

namespace scriptable
{
	// Componentだけに限らず、3DObjectとかNodeにも付けてコールバック実行オブジェクトとして取り扱うことができる抽象クラス
	class CScriptCallback
	{
	public:
#ifdef USE_NETWORK
		virtual void OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData);
#endif // USE_NETWORK
	};
}
