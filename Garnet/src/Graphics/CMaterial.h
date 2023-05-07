#pragma once

#include <vector>
#include <memory>
#include "../Interface/IDescriptor.h"
#include "../Interface/IGraphicsAPI.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CMaterialCreateInfo;
	class IDescriptor;

	class CMaterial
	{
		std::vector<std::shared_ptr<IDescriptor>> m_BindingDescList;
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI, const CMaterialCreateInfo& createInfo) = 0;
		virtual bool Update(float SecondsTime) = 0;

		virtual void AddBindingDescriptor(const std::shared_ptr<IDescriptor>& Desc);
	};
}