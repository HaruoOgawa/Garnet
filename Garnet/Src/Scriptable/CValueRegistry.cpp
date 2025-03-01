#include "CValueRegistry.h"
#include <Scene/CSceneController.h>

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

	void CValueRegistry::OnLoaded(const std::shared_ptr<scene::CSceneController>& SceneController)
	{
		if (m_RegistryName.empty()) return;

		const auto& InitValueRegistryList = SceneController->GetValueRegistryList();

		const auto& it = InitValueRegistryList.find(GetRegistryName());
		if (it != InitValueRegistryList.end())
		{
			const auto& InitValueRegistry = it->second;

			for (const auto& Value : InitValueRegistry->GetValueList())
			{
				SetValue(Value.second.Name, Value.second.Type, &Value.second.Buffer[0], Value.second.ByteSize);
			}
		}

		// 更新
		SceneController->SetValueRegistry(GetRegistryName(), shared_from_this());
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
		if (m_RegistryName.empty()) return;

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
		if (m_RegistryName.empty()) return;

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

	float CValueRegistry::GetValueFloat(const std::string& Key) const
	{
		const auto value = GetValue(Key);
		if (value.Buffer.empty()) return 0.0f;

		if (value.Type != graphics::EUniformValueType::VALUE_TYPE_FLOAT) return 0.0f;

		return *reinterpret_cast<const float*>(value.Buffer.data());
	}

	int CValueRegistry::GetValueInt(const std::string& Key) const
	{
		const auto value = GetValue(Key);
		if (value.Buffer.empty()) return 0;

		if (value.Type != graphics::EUniformValueType::VALUE_TYPE_INT) return 0;

		return static_cast<int>(*reinterpret_cast<const float*>(value.Buffer.data()));
	}

	std::string CValueRegistry::GetValueString(const std::string& Key) const
	{
		const auto value = GetValue(Key);
		if (value.Buffer.empty()) return std::string();

		if (value.Type != graphics::EUniformValueType::VALUE_TYPE_STRING) return std::string();

		std::string str = std::string();
		str.resize(value.Buffer.size());
		std::memcpy(&str[0], &value.Buffer[0], value.ByteSize);

		return str;
	}
}