#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../Graphics/SBindingLayout.h"
#include "../Graphics/SBufferValueLayout.h"
#include "../Graphics/STextureBindingLayout.h"
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

		std::string m_FileName;

		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::vector<SShaderBufferSet> m_ShaderBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;
	public:
		CMaterialFrame();
		virtual ~CMaterialFrame();

		void SetMaterialFrameName(const std::string& Name);
		const std::string& GetMaterialFrameName() const;

		void SetFileName(const std::string& Name);
		const std::string& GetFileName() const;

		void SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo);

		void SetShaderBufferList(const std::vector<SShaderBufferSet>& ShaderBufferList);
		void SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList);

		std::shared_ptr<CMaterial> CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, int RefCount, graphics::ECullMode CullMode);

		bool Reload();
	};
}