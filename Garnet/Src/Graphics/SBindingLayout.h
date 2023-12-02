#pragma once
#include <string>
#include "EBufferUpdateTypeh.h"

namespace graphics
{
	struct SBindingLayout
	{
		std::string BindingName = "";
		int BindingIndex = -1;
		int ByteSize = 0;
		int ByteOffset = 0;
		bool IsGPGPUWritable = false;
		EBufferUpdateType BufferUpdateType = EBufferUpdateType::UPDATE_TYPE_CPU;

		SBindingLayout() :
			BindingName(""), BindingIndex(-1), ByteSize(0), ByteOffset(0), IsGPGPUWritable(false)
		{
		}

		SBindingLayout(const std::string& Name, int Index, bool Writable):
			BindingName(Name), BindingIndex(Index), ByteSize(0), ByteOffset(0), IsGPGPUWritable(Writable)
		{
		}
	};
}