#pragma once

#ifdef USE_NETWORK

#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <memory>

#include "SDMXFixture.h"

namespace scriptable { class CScriptCallback; }

namespace network
{
	class CDMXDataHandler
	{
		std::map<std::tuple<int, int, int>, std::string> m_UniverseDeviceMap;
		std::map<std::string, SDMXFixture> m_DeviceFixtureMap;
		std::map<std::string, std::vector<std::shared_ptr<scriptable::CScriptCallback>>> m_ScriptCallbackMap;
	public:
		CDMXDataHandler();
		virtual ~CDMXDataHandler();

		// フィクスチャを登録
		void RegistDeviceFixture(unsigned short Net, unsigned short SubNet, unsigned short Universe, const SDMXFixture& Fixture);
		
		// 照明灯体を追加する
		void AddDevice(const std::string& DeviceName, const std::shared_ptr<scriptable::CScriptCallback>& Callback);

		// DMXデータを受けて各照明灯体に分配する
		bool DispatchDMXData(unsigned short Net, unsigned short SubNet, unsigned short Universe, const std::vector<unsigned char>& DataBuffer);
	};
}
#endif // USE_NETWORK