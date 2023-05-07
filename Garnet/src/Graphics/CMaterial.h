#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CUniformBufferDescriptor.h"
#include "CTextureBufferDescriptor.h"
#include "../Interface/IGraphicsAPI.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

namespace api { class IGraphicsAPI; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CUniformBufferDescriptor;
	class CTextureBufferDescriptor;

	class CMaterial
	{
	protected:
		std::vector<std::shared_ptr<CUniformBufferDescriptor>> m_UniformBufferDescList;
		std::vector<std::shared_ptr<CTextureBufferDescriptor>> m_TextureBufferDescList;
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI, const CMaterialCreateInfo& createInfo) = 0;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection) = 0;

		virtual void AddUniformBufferDescriptor(const std::shared_ptr<CUniformBufferDescriptor>& Desc);
		virtual void AddTextureBufferDescriptor(const std::shared_ptr<CTextureBufferDescriptor>& Desc);

		virtual void SetUniformValue(const std::string Name, const void* Value) = 0;
	};
}