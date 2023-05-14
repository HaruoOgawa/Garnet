#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CUniformBuffer.h"
#include "CUniformBufferDescriptor.h"
#include "CTextureBuffer.h"
#include "CTextureBufferDescriptor.h"
#include "../Interface/IGraphicsAPI.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

namespace api { class IGraphicsAPI; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CUniformBuffer;
	class CTextureBuffer;

	class CMaterial
	{
	protected:
		std::shared_ptr<CMaterialCreateInfo> m_CreateInfo;

		std::vector<std::shared_ptr<CUniformBuffer>> m_UniformBufferList;
		std::vector<std::shared_ptr<CTextureBuffer>> m_TextureBufferList;

		int											 m_RefCount;
		bool										 m_UseDynamicUniform;
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual void SetCreateInfo(const std::shared_ptr<CMaterialCreateInfo>& createInfo);
		virtual bool Create(api::IGraphicsAPI* pGraphicsAPI) = 0;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection) = 0;
		virtual bool BuildDrawBuffer() = 0;

		virtual void AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer);
		virtual void AddTextureBuffer(const std::shared_ptr<CTextureBuffer>& Buffer);

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum) = 0;

		virtual void IncreaseRefCount();
		virtual int GetRefCount() const;
	};
}