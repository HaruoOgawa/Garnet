#include "CShaderBuffer.h"
#include "CShaderBufferDescriptor.h"
#include "CMaterial.h"
#include "../Debug/Message/Console.h"
#include "../../Math/CMath.h"

namespace graphics
{
	CShaderBuffer::CShaderBuffer(EBufferType BufferType, const std::vector<SBindingLayout>& BindingLayoutList) :
		m_Descriptor(std::make_shared<CShaderBufferDescriptor>()),
		m_BufferType(BufferType)
	{
		for (const auto& Layout : BindingLayoutList)
		{
			m_BindingLayoutList.insert({ Layout.BindingIndex, Layout });
		}
	}

	CShaderBuffer::~CShaderBuffer()
	{
	}

	void CShaderBuffer::AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
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

	void CShaderBuffer::SetData(const std::string& Name, const void* Data, int ByteSize)
	{
		if (m_Descriptor)
		{
			const auto& DataList = m_Descriptor->GetDataList();
			const auto& UniformData = DataList.find(Name);
			if (UniformData != DataList.end())
			{
				const int Offset = UniformData->second.ByteOffset;
				const int Size = UniformData->second.ByteSize;

				if (ByteSize == Size) SetValue(Data, Offset, Size);
			}
		}
	}

	const std::vector<unsigned char>& CShaderBuffer::GetData() const
	{
		return m_Buffer;
	}

	void CShaderBuffer::SetValue(const void* Value, int ByteOffset, int ByteSize)
	{
		std::memcpy(&m_Buffer[ByteOffset], Value, ByteSize);
	}

	std::shared_ptr<CShaderBufferDescriptor> CShaderBuffer::GetDescriptor() const
	{
		return m_Descriptor;
	}

	const std::map<int, SBindingLayout>& CShaderBuffer::GetBindingLayoutList() const
	{
		return m_BindingLayoutList;
	}

	EBufferType CShaderBuffer::GetBufferType() const
	{
		return m_BufferType;
	}

	void CShaderBuffer::RecalculateBindingLayoutOffset()
	{
		for (int i = 1; i < m_BindingLayoutList.size(); i++)
		{
			m_BindingLayoutList[i].ByteOffset = m_BindingLayoutList[i - 1].ByteSize;
		}
	}

	// バッファサイズを2のn乗にする
	void CShaderBuffer::ResizePowerOfTwo()
	{
		const uint64_t ByteSize = static_cast<uint64_t>(math::CMath::CalcNextPowerOfTwo(static_cast<unsigned int>(m_Buffer.size()))); // 2のn乗にする
		m_Buffer.resize(ByteSize, 0);
	}

	void CShaderBuffer::SetSharedBufferParam(const SSharedBufferParam& Param)
	{
		m_SharedBufferParam = Param;
	}

	const SSharedBufferParam& CShaderBuffer::GetSharedBufferParam() const
	{
		return m_SharedBufferParam;
	}
}