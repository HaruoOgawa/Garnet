#pragma once

#include <map>
#include <string>
#include "../Interface/IDescriptor.h"
#include "../Graphics/EUniformValueType.h"
#include "../Graphics/SUniformValueInput.h"

namespace graphics
{
	struct SUniformBufferValue
	{
		std::string UniformName;
		EUniformValueType ValueType;
		int ByteSize;
		int ByteOffset;
		SUniformValueInput ValueInput;
	};

	class CShaderBufferDescriptor : public IDescriptor
	{
		std::map<std::string, SUniformBufferValue> m_DataList;
	public:
		CShaderBufferDescriptor();
		virtual ~CShaderBufferDescriptor();

		void AddData(const std::string& Name, const SUniformBufferValue& Data);
		void SetData(const std::string& Name, const SUniformBufferValue& Data);
		const std::map<std::string, SUniformBufferValue>& GetDataList() const;
	};
}