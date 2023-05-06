#pragma once

#include <map>
#include <string>
#include "../Interface/IBindingDescriptor.h"

namespace graphics
{
	struct SUniformBufferValue
	{
		int Stride;
		int Offset;
	};

	class CUniformBufferDescriptor: public IBindingDescriptor
	{
		std::map<std::string, SUniformBufferValue> m_DataList;
	public:
		CUniformBufferDescriptor();
		virtual ~CUniformBufferDescriptor();

		void AddData(const std::string& Name, const SUniformBufferValue& Data);
	};
}