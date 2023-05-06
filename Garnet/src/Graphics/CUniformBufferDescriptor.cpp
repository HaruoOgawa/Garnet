#include "CUniformBufferDescriptor.h"

namespace graphics
{
	CUniformBufferDescriptor::CUniformBufferDescriptor()
	{
	}

	CUniformBufferDescriptor::~CUniformBufferDescriptor()
	{
		m_DataList.clear();
	}

	void CUniformBufferDescriptor::AddData(const std::string& Name, const SUniformBufferValue& Data)
	{
		m_DataList.insert({ Name, Data });
	}
}