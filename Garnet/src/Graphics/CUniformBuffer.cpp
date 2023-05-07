#include "CUniformBuffer.h"
#include "CUniformBufferDescriptor.h"
#include "../Debug/Message/Console.h"

namespace graphics
{
	CUniformBuffer::CUniformBuffer(std::vector<int> BindIndexList):
		m_Descriptor(std::make_shared<CUniformBufferDescriptor>()),
		m_BufferType(EBufferType::UNIFROM)
	{
		m_BindingLayoutList.resize(BindIndexList.size());

		for (int i = 0; i < m_BindingLayoutList.size(); i++)
		{
			m_BindingLayoutList[i].BindingIndex = BindIndexList[i];
		}
	}

	CUniformBuffer::~CUniformBuffer()
	{
	}

	void CUniformBuffer::AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
	{
		if (BindingIndex < 0 || BindingIndex >= m_BindingLayoutList.size())
		{
			Console::Log("[ERROR] BindingIndex is out of range. %d\n", BindingIndex);
			return;
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

	std::shared_ptr<CUniformBufferDescriptor> CUniformBuffer::GetDescriptor() const
	{
		return m_Descriptor;
	}

	const std::vector<SBindingLayout>& CUniformBuffer::GetBindingLayoutList() const
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