#include "CShaderBufferDescriptor.h"

namespace graphics
{
	CShaderBufferDescriptor::CShaderBufferDescriptor()
	{
	}

	CShaderBufferDescriptor::~CShaderBufferDescriptor()
	{
		m_DataList.clear();
	}

	void CShaderBufferDescriptor::AddData(const std::string& Name, const SUniformBufferValue& Data)
	{
		m_DataList.insert({ Name, Data });
	}

	const std::map<std::string, SUniformBufferValue>& CShaderBufferDescriptor::GetDataList() const
	{
		return m_DataList;
	}
}