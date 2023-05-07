#pragma once

#include <map>
#include <string>
#include "../Interface/IDescriptor.h"

namespace graphics
{
	class CTextureBufferDescriptor : public IDescriptor
	{
	public:
		CTextureBufferDescriptor();
		virtual ~CTextureBufferDescriptor();
	};
}