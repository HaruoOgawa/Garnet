#pragma once

// UniformBuffer と SSBO(Shader Storage Buffer Object) を取り扱うバッファ

#include "../Interface/IBuffer.h"
#include <vector>
#include <map>
#include <memory>
#include "SBindingLayout.h"
#include "EBufferUpdateTypeh.h"

namespace graphics
{
	class CShaderBufferDescriptor;
	class CMaterial;

	struct SSharedBufferParam
	{
		bool IsShared = false;
		int BufferIndex = -1;
		std::shared_ptr<CMaterial> SharedBufferMaterial = nullptr;
	};

	class CShaderBuffer : public IBuffer
	{
		std::vector<unsigned char> m_Buffer;
		std::shared_ptr<CShaderBufferDescriptor> m_Descriptor;
		std::map<int, SBindingLayout> m_BindingLayoutList;

		const EBufferType m_BufferType;
		const EBufferUpdateType m_BufferUpdateType;

		// バッファを共有するかどうか
		SSharedBufferParam m_SharedBufferParam{};
	public:
		CShaderBuffer(EBufferType BufferType, const std::vector<SBindingLayout>& BindingLayoutList, EBufferUpdateType BufferUpdateType);
		virtual ~CShaderBuffer();

		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) override;
		virtual void SetData(const std::string& Name, const void* Data, int ByteSize) override;
		virtual const std::vector<unsigned char>& GetData() const override;

		void SetValue(const void* Value, int ByteOffset, int ByteSize);

		virtual std::shared_ptr<CShaderBufferDescriptor> GetDescriptor() const;
		virtual const std::map<int, SBindingLayout>& GetBindingLayoutList() const;

		virtual EBufferType GetBufferType() const override;

		virtual void RecalculateBindingLayoutOffset() override;

		// バッファサイズを2のn乗にする
		virtual void ResizePowerOfTwo() override;

		void SetSharedBufferParam(const SSharedBufferParam& Param);
		const SSharedBufferParam& GetSharedBufferParam() const;

		EBufferUpdateType GetBufferUpdateType() const;
	};
}