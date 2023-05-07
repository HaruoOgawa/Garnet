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

	class CUniformBufferDescriptor : public IDescriptor
	{
		std::map<std::string, SUniformBufferValue> m_DataList;
	public:
		CUniformBufferDescriptor();
		virtual ~CUniformBufferDescriptor();

		void AddData(const std::string& Name, const SUniformBufferValue& Data);
	};
}