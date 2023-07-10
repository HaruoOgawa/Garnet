#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CUniformBuffer.h"
#include "CUniformBufferDescriptor.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"
#include "../File/CFileReader.h"
#include "../../Interface/IGraphicsAPI.h"
#include "STextureBindingLayout.h"
#include "CTexture.h"
#include "CDrawInfo.h"

namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }

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
		bool									     m_UseDynamicUniform;

		std::vector<uint32_t> m_BindingRefSizeList; // GLSLの各bindingが参照しているバッファのサイズ

		std::shared_ptr<graphics::CMaterial> m_DepthMaterial;
	public:
		CMaterial();
		virtual ~CMaterial() = default;

		virtual void SetCreateInfo(const std::shared_ptr<CMaterialCreateInfo>& createInfo);

		virtual bool Create(const std::vector<std::shared_ptr<graphics::CTexture>>& TextureList, const std::vector<std::shared_ptr<graphics::CTexture>>& CubeMapList) = 0;
		virtual bool CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<file::CFileReader>& DepthVertex, const std::shared_ptr<file::CFileReader>& DepthFragment);

		virtual std::shared_ptr<graphics::CMaterial> GetDepthMaterial();

		virtual bool SetCommonUniform(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) = 0;
		virtual bool BuildDrawBuffer(int DynamicOffsetNum) = 0;

		virtual void AddUniformBuffer(const std::shared_ptr<CUniformBuffer>& Buffer);
		virtual void AddTextureBindingLayout(const STextureBindingLayout& Layout);

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum) = 0;

		virtual void IncreaseRefCount();
		virtual int GetRefCount() const;
		virtual void SetRefStatus(int RefCount, bool UseDynamicUniform);

		virtual const std::vector<uint32_t>& GetBindingRefSizeList() const;

		virtual bool IsUseDynamicUniform() const;
	};
}