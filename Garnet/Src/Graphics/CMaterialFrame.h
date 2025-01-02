#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Graphics/SBindingLayout.h"
#include "../Graphics/SBufferValueLayout.h"
#include "../Graphics/STextureBindingLayout.h"
#include "../Graphics/ECullMode.h"
#include "../Graphics/EDepthFunc.h"
#include "../Graphics/SStencilParam.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

#include "CMaterial.h"
#include "../Interface/IGraphicsAPI.h"

namespace graphics
{
	struct SShaderBufferSet
	{
		graphics::EBufferType BufferType;
		graphics::SBindingLayout BindingLayout;
		std::vector<std::shared_ptr<graphics::SBufferValueLayout>> ValueLayoutList;
	};

	class CMaterialFrame : public std::enable_shared_from_this<CMaterialFrame>
	{
		std::vector<std::shared_ptr<CMaterial>> m_RefMaterialList;

		int m_CreateCounter;

		std::string m_MaterialFrameName;

		// DepthTest
		bool m_EnabledZWrite;
		graphics::EDepthFunc m_DepthFunc;

		// StencilTest
		graphics::SStencilParam m_StencilParam;

		// カリング
		graphics::ECullMode m_CullMode;

		std::string m_FileName;

		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::vector<SShaderBufferSet> m_ShaderBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;

		// カラーバッファへのアウトプット数(MRTで使用)
		int m_OutputColorCount;
	public:
		CMaterialFrame();
		virtual ~CMaterialFrame();

		void SetMaterialFrameName(const std::string& Name);
		const std::string& GetMaterialFrameName() const;

		void SetEnabledZWrite(bool ZWrite);
		bool IsEnabledZWrite() const;

		void SetDepthFunc(graphics::EDepthFunc DepthFunc);
		graphics::EDepthFunc GetDepthFunc() const;

		void SetStencilParam(const SStencilParam& Param);
		const SStencilParam& GetStencilParam() const;

		void SetCullMode(graphics::ECullMode CullMode);
		graphics::ECullMode GetCullMode() const;

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		bool GetDefaultValue(const std::string& BufferName, const std::string& UniformName, std::vector<float>& DstData) const;

		// カラーバッファへのアウトプット数(MRTで使用)
		void SetOutputColorCount(int Val);
		int GetOutputColorCount() const;

		void SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo);

		void SetShaderBufferList(const std::vector<SShaderBufferSet>& ShaderBufferList);
		void SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList);

		std::shared_ptr<CMaterial> CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, graphics::ECullMode CullMode);
		std::shared_ptr<CMaterial> CopyMaterial(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CMaterial>& SrcMaterial);
		bool DeleteRefMaterial(const std::shared_ptr<graphics::CMaterial>& Material);

		bool Reload();
	};
}