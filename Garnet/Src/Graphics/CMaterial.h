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
#include "EDepthFunc.h"

#include "../Camera/CCamera.h"

namespace camera { class CCamera; }
namespace projection { class CProjection; }

namespace graphics
{
	class CMaterialCreateInfo;
	class CShaderBuffer;
	class CMaterialFrame;
	class CDrawInfo;

	class CMaterial : public std::enable_shared_from_this<CMaterial>
	{
	protected:
		std::shared_ptr<CMaterialFrame> m_MaterialFrame;

		std::string m_MaterialName;

		std::shared_ptr<CMaterialCreateInfo> m_CreateInfo;

		std::vector<std::shared_ptr<CShaderBuffer>> m_ShaderBufferList;
		std::vector<STextureBindingLayout> m_TextureBindingLayoutList;

		// VulkanやWebGPUはOpenGLの様に何も考えずにマテリアルを使いまわすことができないのでその数をあらかじめ設定しておく必要がある
		const int m_RefCount;
		int m_CurrentDynamicOffset;

		std::vector<uint32_t> m_BindingRefSizeList; // GLSLの各bindingが参照しているバッファのサイズ

		std::shared_ptr<graphics::CMaterial> m_DepthMaterial;

		bool m_EnabledZWrite;
		EDepthFunc m_DepthFunc;

		const ECullMode m_DefaultCullMode;
		ECullMode m_CullMode;
		
		EBlendType m_BlendType;

		bool m_IsDrawOutline;

		// Texture
		std::shared_ptr<graphics::CTexture> m_EmptyTexture;
		std::shared_ptr<graphics::CTexture> m_EmptyCubeTexture;

		// 参照中のテクスチャリスト
		std::map<std::string, std::shared_ptr<graphics::CTexture>> m_RefTextureMap;
		std::map<std::string, std::shared_ptr<graphics::CTexture>> m_RefCubeMapMap;
		std::map<std::string, std::shared_ptr<graphics::CTexture>> m_RefFrameTextureMap;
		std::shared_ptr<graphics::CTexture> m_RefDiffuse_Tex = nullptr;
		std::shared_ptr<graphics::CTexture> m_RefSpecular_Tex = nullptr;
		std::shared_ptr<graphics::CTexture> m_RefGGXLUT_Tex = nullptr;
	public:
		CMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterialCreateInfo>& createInfo, int RefCount, ECullMode CullMode);
		virtual ~CMaterial() = default;

		virtual bool IsUseShaderBuffer();

		virtual void SetMaterialName(const std::string& Name);
		virtual const std::string& GetMaterialName() const;

		virtual const std::shared_ptr<CMaterialFrame>& GetMaterialFrame() const;
		virtual void SetMaterialFrame(const std::shared_ptr<CMaterialFrame>& MaterialFrame);
		virtual bool DeleteMaterialFrameReference();

		virtual std::vector<std::shared_ptr<CShaderBuffer>>& GetShaderBufferList();
		virtual const std::vector<STextureBindingLayout>& GetTextureBindingLayoutList() const;
		virtual void SetTextureBindingLayoutTextureIndex(int BindingLayoutIndex, int TextureIndex, const std::shared_ptr<graphics::CTextureSet>& TextureSet);

		virtual bool Create(const std::shared_ptr<graphics::CTextureSet>& TextureSet) = 0;
		virtual bool CreateDepthMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterialFrame>& DepthMF);

		virtual bool ReCreate(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::vector<std::shared_ptr<CShaderBuffer>>& ShaderBufferList, const std::vector<STextureBindingLayout>& TextureBindingLayoutList);

		virtual bool ReCreateBuffer(const std::vector<std::shared_ptr<graphics::CShaderBuffer>>& ShaderBufferList, const std::vector<graphics::STextureBindingLayout>& TextureBindingLayoutList);

		virtual bool CreateRefTextureList(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, const std::shared_ptr<graphics::CTextureSet>& TextureSet);
		virtual bool ReCreateRefTextureList(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo);

		virtual std::shared_ptr<graphics::CMaterial> GetDepthMaterial();

		virtual void SetEnabledZWrite(bool Flag);
		virtual bool IsEnabledZWrite() const;

		virtual void SetDepthFunc(EDepthFunc DepthFunc);
		virtual EDepthFunc GetDepthFunc() const;

		virtual void SetCullMode(ECullMode CullMode);
		virtual ECullMode GetCullMode() const;
		virtual void ResetToDefaultCullMode();
		
		virtual void SetBlendType(EBlendType BlendType);
		virtual EBlendType GetBlendType() const;

		virtual bool BuildDrawBuffer(int DynamicOffsetNum) = 0;

		virtual void AddShaderBuffer(const std::shared_ptr<CShaderBuffer>& Buffer);
		virtual void AddTextureBindingLayout(const STextureBindingLayout& Layout);
		virtual void ReplaceTextureIndex(const std::string& TextureName, int TextureIndex);

		virtual void ReplacePreloadUniformValue(const std::string& Name, const void* Data, int ByteSize, int BindingIndex);

		virtual void SetUniformValue(const std::string Name, const void* Data, int ByteSize, int DynamicOffsetNum = -1) = 0;

		virtual int GetRefCount() const;

		virtual bool IsUseDynamicOffset();

		virtual void IncreaseDynamicOffset();
		virtual int GetDynamicOffset() const;
		virtual void ResetDynamicOffset();

		virtual const std::vector<uint32_t>& GetBindingRefSizeList() const;

		virtual void SetIsDrawOutline(bool Frag);
		virtual bool IsDrawOutline() const;
	};
}