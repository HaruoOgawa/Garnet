#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../Interface/IDescriptor.h"

namespace graphics
{
	struct SBindingLayout;

	enum class EBufferType
	{
		UNIFORM,
		TEXTURE,
		SHADERSTORAGE, // SSBO(Shader Storage Buffer Object)
	};

	class IBuffer
	{
	public:
		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) = 0;
		virtual void SetData(const std::string& Name, const void* Data, int ByteSize) = 0;
		virtual const std::vector<unsigned char>& GetData() const = 0;

		virtual EBufferType GetBufferType() const = 0;

		virtual void RecalculateBindingLayoutOffset() = 0;

		// バッファサイズを2のn乗にする
		virtual void ResizePowerOfTwo() = 0;
	};
}