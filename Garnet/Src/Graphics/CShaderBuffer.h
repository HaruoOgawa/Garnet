#pragma once

// UniformBuffer と SSBO(Shader Storage Buffer Object) を取り扱うバッファ

#include "../Interface/IBuffer.h"
#include <vector>
#include <map>
#include <memory>
#include "SBindingLayout.h"

namespace graphics
{
	class CShaderBufferDescriptor;

	class CShaderBuffer : public IBuffer
	{
		std::vector<unsigned char> m_Buffer;
		std::shared_ptr<CShaderBufferDescriptor> m_Descriptor;
		std::map<int, SBindingLayout> m_BindingLayoutList;

		const EBufferType m_BufferType;
	public:
		CShaderBuffer(EBufferType BufferType, const std::vector<SBindingLayout>& BindingLayoutList);
		virtual ~CShaderBuffer();

		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) override;
		virtual void SetData(const std::string& Name, const void* Data, int ByteSize) override;
		virtual const std::vector<unsigned char>& GetData() const override;

		void SetValue(const void* Value, int ByteOffset, int ByteSize);

		virtual std::shared_ptr<CShaderBufferDescriptor> GetDescriptor() const;
		virtual const std::map<int, SBindingLayout>& GetBindingLayoutList() const;

		virtual EBufferType GetBufferType() const override;

		virtual void RecalculateBindingLayoutOffset() override;
	};
}