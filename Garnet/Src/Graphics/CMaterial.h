#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CUniformBuffer.h"
#include "CUniformBufferDescriptor.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"
#include "STextureBindingLayout.h"
#include "CTexture.h"

namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CUniformBuffer;

	class CMaterial
	{
	protected:
		std::shared_ptr<CMaterialCreateInfo> m_CreateInfo;

		std::vector<std::shared_ptr<CUniformBuffer>> m_UniformBufferList;
		std::vector<STextureBindingLayout> m_TextureBindingLayoutList;

		int											 m_RefCount;
		const bool								     m_UseDynamicUniform;

		std::vector<uint32_t> m_BindingRefSizeList; // GLSLの各bindingが参照しているバッファのサイズ
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual void SetCreateInfo(const std::shared_ptr<CMaterialCreateInfo>& createInfo);
		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList) = 0;
		virtual bool Update(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection) = 0;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) = 0;

		virtual void AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer);
		virtual void AddTextureBindingLayout(const STextureBindingLayout& Layout);

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum) = 0;

		virtual void IncreaseRefCount();
		virtual int GetRefCount() const;

		virtual const std::vector<uint32_t>& GetBindingRefSizeList() const;

		virtual bool IsUseDynamicUniform() const;
	};
}