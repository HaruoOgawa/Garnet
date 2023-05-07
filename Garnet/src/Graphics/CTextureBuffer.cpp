#include "CTextureBuffer.h"
#include "CTextureBufferDescriptor.h"

namespace graphics
{
	CTextureBuffer::CTextureBuffer(std::vector<int> BindIndexList):
		m_Descriptor(std::make_shared<CTextureBufferDescriptor>()),
		m_BufferType(EBufferType::TEXTURE)
	{
		m_BindingLayoutList.resize(BindIndexList.size());

		for (int i = 0; i < m_BindingLayoutList.size(); i++)
		{
			m_BindingLayoutList[i].BindingIndex = BindIndexList[i];
		}
	}
	CTextureBuffer::~CTextureBuffer()
	{
	}

	void CTextureBuffer::AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex)
	{
	}

	const std::vector<unsigned char>& CTextureBuffer::GetData() const
	{
		return m_Buffer;
	}

	std::shared_ptr<CTextureBufferDescriptor> CTextureBuffer::GetDescriptor() const
	{
		return m_Descriptor;
	}

	const std::vector<SBindingLayout>& CTextureBuffer::GetBindingLayoutList() const
	{
		return m_BindingLayoutList;
	}

	EBufferType CTextureBuffer::GetBufferType() const
	{
		return m_BufferType;
	}

	void CTextureBuffer::RecalculateBindingLayoutOffset()
	{
	}
}