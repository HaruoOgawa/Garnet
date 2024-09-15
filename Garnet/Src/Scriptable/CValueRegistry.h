#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>
#include "../Graphics/EUniformValueType.h"
#include <glm/glm.hpp>

namespace scriptable
{
	struct SValue
	{
		std::string Name = std::string();
		graphics::EUniformValueType Type = graphics::EUniformValueType::NONE;
		int ByteSize = 0;
		std::vector<unsigned char> Buffer;
	};

	class CValueRegistry : public std::enable_shared_from_this<CValueRegistry>
	{
		const std::string m_RegistryName;

		std::map<std::string, SValue> m_ValueList;
		
	public:
		explicit CValueRegistry(const std::string& RegistryName);
		virtual ~CValueRegistry();

		const std::string& GetRegistryName() const;

		const std::map<std::string, SValue>& GetValueList() const;

		void SetValue(const std::string& Key, graphics::EUniformValueType ValueType, const void* Data, int ByteSize);
		SValue GetValue(const std::string& Key) const;
	};
}