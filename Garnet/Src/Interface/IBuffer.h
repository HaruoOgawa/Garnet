#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../Interface/IDescriptor.h"

namespace graphics
{
	struct SBindingLayout;

	enum class EBufferType
	{
		UNIFROM,
		TEXTURE,
	};

	class IBuffer
	{
	public:
		virtual void AddData(const std::string& Name, const void* Data, int ByteSize, int BindingIndex) = 0;
		virtual const std::vector<unsigned char>& GetData() const = 0;

		virtual const std::vector<SBindingLayout>& GetBindingLayoutList() const = 0;

		virtual EBufferType GetBufferType() const = 0;

		virtual void RecalculateBindingLayoutOffset() = 0;
	};
}