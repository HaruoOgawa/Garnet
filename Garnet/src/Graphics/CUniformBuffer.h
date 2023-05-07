#pragma once

#include "../Interface/IBuffer.h"
#include <vector>
#include <memory>
#include "SBindingLayout.h"

namespace graphics
{
	class CUniformBufferDescriptor;

	class CUniformBuffer : public IBuffer
	{
		std::vector<unsigned char> m_Buffer;
		std::shared_ptr<CUniformBufferDescriptor> m_Descriptor;
		std::vector<SBindingLayout> m_BindingLayoutList;

		const EBufferType m_BufferType;
	public:
		explicit CUniformBuffer(std::vector<int> BindIndexList);
		virtual ~CUniformBuffer();

		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) override;
		virtual const std::vector<unsigned char>& GetData() const override;

		virtual std::shared_ptr<CUniformBufferDescriptor> GetDescriptor() const;
		virtual const std::vector<SBindingLayout>& GetBindingLayoutList() const override;

		virtual EBufferType GetBufferType() const override;

		virtual void RecalculateBindingLayoutOffset() override;
	};
}