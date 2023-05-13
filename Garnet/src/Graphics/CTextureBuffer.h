#pragma once

#include "../Interface/IBuffer.h"
#include <vector>
#include <memory>
#include "SBindingLayout.h"

namespace graphics
{
	class CTextureBufferDescriptor;

	class CTextureBuffer : public IBuffer
	{
		std::vector<unsigned char> m_Buffer;
		std::shared_ptr<CTextureBufferDescriptor> m_Descriptor;
		std::vector<SBindingLayout> m_BindingLayoutList;

		const EBufferType m_BufferType;
	public:
		explicit CTextureBuffer(std::vector<int> BindIndexList);
		virtual ~CTextureBuffer();

		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) override;
		virtual const std::vector<unsigned char>& GetData() const override;

		virtual std::shared_ptr<CTextureBufferDescriptor> GetDescriptor() const;
		virtual const std::vector<SBindingLayout>& GetBindingLayoutList() const override;

		virtual EBufferType GetBufferType() const override;

		virtual void RecalculateBindingLayoutOffset() override;
	};
}