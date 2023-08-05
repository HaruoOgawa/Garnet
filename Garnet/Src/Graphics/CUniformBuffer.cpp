#include "CUniformBuffer.h"
#include "CUniformBufferDescriptor.h"
#include "../Debug/Message/Console.h"

namespace graphics
{
	CUniformBuffer::CUniformBuffer(const std::vector<SBindingLayout>& BindingLayoutList) :
		m_Descriptor(std::make_shared<CUniformBufferDescriptor>()),
		m_BufferType(EBufferType::UNIFROM)
	{
		for (const auto& Layout : BindingLayoutList)
		{
			m_BindingLayoutList.insert({ Layout.BindingIndex, Layout });
		}
	}

	CUniformBuffer::~CUniformBuffer()
	{
	}

	void CUniformBuffer::AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
	{
		const auto& Layout = m_BindingLayoutList.find(BindingIndex);

		if (BindingIndex < 0 || Layout == m_BindingLayoutList.end())
		{
			Console::Log("[ERROR] BindingIndex is out of range. %d\n", BindingIndex);
			return;
		}
		else if (BindingIndex >= 1 && m_BindingLayoutList.size() >= 2 && m_BindingLayoutList[BindingIndex - 1].ByteSize < 256)
		{
			Console::Log("[WARNING] UBO(%d) is smaller than 256 byte.(UBO ByteSize: %d) But you are going to use the following buffer.\n",
				BindingIndex - 1, m_BindingLayoutList[BindingIndex - 1].ByteSize);
		}

		// BUFFER
		int ByteOffset = static_cast<int>(m_Buffer.size());
		m_Buffer.resize(m_Buffer.size() + ByteSize);
		std::memcpy(&m_Buffer[ByteOffset], Data, ByteSize);

		// DESCRIPTOR
		SUniformBufferValue value = {
			ByteSize,
			ByteOffset
		};
		m_Descriptor->AddData(Name, value);

		// BINDING LAYOUT
		m_BindingLayoutList[BindingIndex].ByteSize += ByteSize;
	}

	const std::vector<unsigned char>& CUniformBuffer::GetData() const
	{
		return m_Buffer;
	}

	void CUniformBuffer::SetValue(const void* Value, int ByteOffset, int ByteSize)
	{
		std::memcpy(&m_Buffer[ByteOffset], Value, ByteSize);
	}

	std::shared_ptr<CUniformBufferDescriptor> CUniformBuffer::GetDescriptor() const
	{
		return m_Descriptor;
	}

	const std::map<int, SBindingLayout>& CUniformBuffer::GetBindingLayoutList() const
	{
		return m_BindingLayoutList;
	}

	EBufferType CUniformBuffer::GetBufferType() const
	{
		return m_BufferType;
	}

	void CUniformBuffer::RecalculateBindingLayoutOffset()
	{
		for (int i = 1; i < m_BindingLayoutList.size(); i++)
		{
			m_BindingLayoutList[i].ByteOffset = m_BindingLayoutList[i - 1].ByteSize;
		}
	}
}