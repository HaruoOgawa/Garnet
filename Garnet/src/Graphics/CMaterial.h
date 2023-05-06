#pragma once

#include <vector>
#include <memory>
#include "../GraphicsAPI/CMaterialCreateInfo.h"

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CUniformBufferDescriptor;

	class CMaterial
	{
		std::vector<std::shared_ptr<CUniformBufferDescriptor>> m_UBODescList;
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI, const CMaterialCreateInfo& createInfo) = 0;
		virtual bool Update(float SecondsTime) = 0;
	};
}