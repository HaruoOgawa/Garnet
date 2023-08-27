#pragma once

#include <map>
#include <string>
#include "../Interface/IDescriptor.h"

namespace graphics
{
	struct SUniformBufferValue
	{
		int ByteSize;
		int ByteOffset;
	};

	class CShaderBufferDescriptor : public IDescriptor
	{
		std::map<std::string, SUniformBufferValue> m_DataList;
	public:
		CShaderBufferDescriptor();
		virtual ~CShaderBufferDescriptor();

		void AddData(const std::string& Name, const SUniformBufferValue& Data);
		const std::map<std::string, SUniformBufferValue>& GetDataList() const;
	};
}