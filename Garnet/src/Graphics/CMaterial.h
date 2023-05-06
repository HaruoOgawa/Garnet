#pragma once

#include <vector>

namespace graphics
{
	class CUniformBufferDescriptor;

	class CMaterial
	{
		std::vector<CUniformBufferDescriptor> m_UBODescList;
	public:
		CMaterial();
		virtual ~CMaterial();
	};
}