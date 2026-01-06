#ifdef USE_NETWORK
#include "CDMXDataHandler.h"
#include "../../Scriptable/CScriptCallback.h"

namespace network
{
	CDMXDataHandler::CDMXDataHandler():
		m_CurrentTimeCode(0.0f)
	{
	}

	CDMXDataHandler::~CDMXDataHandler()
	{
	}

	// フィクスチャを登録
	void CDMXDataHandler::RegistDeviceFixture(unsigned short Net, unsigned short SubNet, unsigned short Universe, const SDMXFixture& Fixture)
	{
		m_UniverseDeviceMap.emplace(std::make_tuple(Net, SubNet, Universe), Fixture.DeviceName);
		m_DeviceFixtureMap.emplace(Fixture.DeviceName, Fixture);
	}

	// 照明灯体を追加する
	void CDMXDataHandler::AddDevice(const std::string& DeviceName, const std::shared_ptr<scriptable::CScriptCallback>& Callback)
	{
		const auto& it = m_ScriptCallbackMap.find(DeviceName);
		if (it == m_ScriptCallbackMap.end()) m_ScriptCallbackMap.emplace(DeviceName, std::vector<std::shared_ptr<scriptable::CScriptCallback>>());

		m_ScriptCallbackMap[DeviceName].push_back(Callback);
	}

	// DMXデータを受けて各照明灯体に分配する
	bool CDMXDataHandler::DispatchDMXData(unsigned short Net, unsigned short SubNet, unsigned short Universe, const std::vector<unsigned char>& DataBuffer)
	{
		// データ取得
		const auto& DeviceIT = m_UniverseDeviceMap.find(std::make_tuple(Net, SubNet, Universe));
		if (DeviceIT == m_UniverseDeviceMap.end()) return false;

		const auto& FixtureIT = m_DeviceFixtureMap.find(DeviceIT->second);
		if (FixtureIT == m_DeviceFixtureMap.end()) return false;

		const auto& ScriptCallbackList = m_ScriptCallbackMap.find(DeviceIT->second);
		if (ScriptCallbackList == m_ScriptCallbackMap.end()) return false;

		const int ChennelCount = static_cast<int>(FixtureIT->second.ChannelNameList.size());
		const int DeviceCount = static_cast<int>(ScriptCallbackList->second.size());

		// 事前にデータサイズを確認
		int TimeCodeByte = 4;
		int DeviceByteSize = sizeof(unsigned char) * ChennelCount;
		int ExpectedByteSize = DeviceByteSize * DeviceCount + TimeCodeByte;
		
		int RealByteSize = static_cast<int>(DataBuffer.size());

		if (ExpectedByteSize > RealByteSize) return false;

		// タイムコードをチェックして古いデータは捨てる
		{
			unsigned char Frame = DataBuffer[0];
			unsigned char Second = DataBuffer[1];
			unsigned char Minute = DataBuffer[2];
			unsigned char Hour = DataBuffer[3];

			float TimeCode = ((float)Hour) * 60.0f * 60.0f + ((float)Minute) * 60.0f + ((float)Second) + ((float)Frame) / 30.0f;
			if (TimeCode < m_CurrentTimeCode) return true;
			m_CurrentTimeCode = TimeCode;
		}

		// データ解析
		// 各デバイスにバイト列を分けて送信
		int ByteOffset = 0;

		// タイムコード分だけ飛ばす
		ByteOffset += TimeCodeByte;

		for (int i = 0; i < DeviceCount; i++)
		{
			auto& Callback = ScriptCallbackList->second[i];

			std::vector<unsigned char> DataPerDevice;
			DataPerDevice.resize(DeviceByteSize);

			std::memcpy(&DataPerDevice[0], &DataBuffer[ByteOffset], DeviceByteSize);

			// DMXデータをデバイスに渡す
			Callback->OnReceiveDMXData(FixtureIT->second, DataPerDevice);

			// オフセット更新
			ByteOffset += DeviceByteSize;
		}

		return true;
	}
}
#endif // USE_NETWORK