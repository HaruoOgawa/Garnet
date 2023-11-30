#pragma once

#include <vector>
#include <memory>
#include <string>
#include "CShaderBuffer.h"
#include "CShaderBufferDescriptor.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"
#include "../LoadWorker/CFile.h"
#include "../../Interface/IGraphicsAPI.h"
#include "STextureBindingLayout.h"
#include "CTextureSet.h"
#include "CDrawInfo.h"
#include "ECullMode.h"
#include "EBlendType.h"
#include "../Camera/CCamera.h"

namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CShaderBuffer;

	class CMaterial
	{
	protected:
		std::shared_ptr<CMaterialCreateInfo> m_CreateInfo;

		std::vector<std::shared_ptr<CShaderBuffer>> m_ShaderBufferList;
		std::vector<STextureBindingLayout> m_TextureBindingLayoutList;

		int											 m_RefCount;
		bool									     m_UseDynamicBufferOffset;

		std::vector<uint32_t> m_BindingRefSizeList; // GLSLの各bindingが参照しているバッファのサイズ

		std::shared_ptr<graphics::CMaterial> m_DepthMaterial;

		bool m_EnabledZTest;
		ECullMode m_CullMode;
		EBlendType m_BlendType;
	public:
		CMaterial(const std::shared_ptr<CMaterialCreateInfo>& createInfo);
		virtual ~CMaterial() = default;

		virtual bool Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet) = 0;
		virtual bool CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<resource::CFile>& DepthVertex, const std::shared_ptr<resource::CFile>& DepthFragment);

		virtual std::shared_ptr<graphics::CMaterial> GetDepthMaterial();

		virtual void SetEnabledZTest(bool EnabledZTest);
		virtual bool IsEnabledZTest() const;

		virtual void SetCullMode(ECullMode CullMode);
		virtual ECullMode GetCullMode() const;
		
		virtual void SetBlendType(EBlendType BlendType);
		virtual EBlendType GetBlendType() const;

		virtual bool BuildDrawBuffer(int DynamicOffsetNum) = 0;

		virtual void AddShaderBuffer(const std::shared_ptr<CShaderBuffer>& Buffer);
		virtual void AddTextureBindingLayout(const STextureBindingLayout& Layout);

		virtual void SetUniformValue(const std::string Name, const void* Value, int DynamicOffsetNum = -1) = 0;

		virtual void IncreaseRefCount();
		virtual int GetRefCount() const;
		virtual void SetRefStatus(int RefCount, bool UseDynamicBufferOffset);

		virtual const std::vector<uint32_t>& GetBindingRefSizeList() const;

		virtual bool IsUseDynamicBufferOffset() const;
	};
}