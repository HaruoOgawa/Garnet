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

	class CMaterialFrame
	{
		std::vector<std::shared_ptr<CMaterial>> m_RefMaterialList;

		int m_CreateCounter;

		std::string m_MaterialName;

		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::vector<SShaderBufferSet> m_ShaderBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;
	public:
		CMaterialFrame();
		virtual ~CMaterialFrame();

		void SetMaterialName(const std::string& Name);

		void SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo);

		void SetShaderBufferList(const std::vector<SShaderBufferSet>& ShaderBufferList);
		void SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList);

		std::shared_ptr<CMaterial> CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, int RefCount, graphics::ECullMode CullMode);

		bool Reload();
	};
}