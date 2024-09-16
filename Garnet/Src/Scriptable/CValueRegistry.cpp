#include "CValueRegistry.h"

namespace scriptable
{
	CValueRegistry::CValueRegistry(const std::string& RegistryName):
		timeline::CTimelineTrackContent(),
		m_RegistryName(RegistryName)
	{
	}

	CValueRegistry::~CValueRegistry()
	{
	}

	const std::string& CValueRegistry::GetRegistryName() const
	{
		return m_RegistryName;
	}

	const std::map<std::string, SValue>& CValueRegistry::GetValueList() const
	{
		return m_ValueList;
	}

	void CValueRegistry::SetValue(const std::string& Key, graphics::EUniformValueType ValueType, const void* Data, int ByteSize)
	{
		const auto it = m_ValueList.find(Key);
		if (it == m_ValueList.end())
		{
			// 新規追加
			m_ValueList.emplace(Key, SValue());
		}

		// 値更新
		auto& Value = m_ValueList[Key];

		Value.Name = Key;
		Value.Type = ValueType;
		Value.ByteSize = ByteSize;

		Value.Buffer.clear();
		Value.Buffer.shrink_to_fit();
		Value.Buffer.resize(ByteSize);
		std::memcpy(&Value.Buffer[0], Data, ByteSize);
	}

	void CValueRegistry::SetValue(const std::string& Key, const void* Data)
	{
		const auto it = m_ValueList.find(Key);
		if (it == m_ValueList.end()) return;

		// 値更新
		auto& Value = m_ValueList[Key];

		Value.Buffer.clear();
		Value.Buffer.shrink_to_fit();
		Value.Buffer.resize(Value.ByteSize);
		std::memcpy(&Value.Buffer[0], Data, Value.ByteSize);
	}

	SValue CValueRegistry::GetValue(const std::string& Key) const
	{
		const auto it = m_ValueList.find(Key);
		if (it == m_ValueList.end()) return {};

		return it->second;
	}
}