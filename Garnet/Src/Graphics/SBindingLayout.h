#pragma once
#include <string>

namespace graphics
{
	struct SBindingLayout
	{
		std::string BindingName = "";
		int BindingIndex = -1;
		int ByteSize = 0;
		int ByteOffset = 0;

		SBindingLayout() :
			BindingName(""), BindingIndex(-1), ByteSize(0), ByteOffset(0)
		{
		}

		SBindingLayout(const std::string& Name, int Index):
			BindingName(Name), BindingIndex(Index), ByteSize(0), ByteOffset(0)
		{
		}
	};
}