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

	void CShaderBufferDescriptor::SetData(const std::string& Name, const SUniformBufferValue& Data)
	{
		if (m_DataList.find(Name) == m_DataList.end()) return;

		m_DataList[Name] = Data;
	}

	const std::map<std::string, SUniformBufferValue>& CShaderBufferDescriptor::GetDataList() const
	{
		return m_DataList;
	}
}