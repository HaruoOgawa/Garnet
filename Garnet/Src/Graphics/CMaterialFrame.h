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
	class CMaterialFrame
	{
		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>> m_UniformBufferList;
		std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>> m_StorageBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;
	public:
		CMaterialFrame();
		virtual ~CMaterialFrame();

		void SetCreateInfo(const std::shared_ptr<graphics::CMaterialCreateInfo>& CreateInfo);

		void SetUniformBufferList(const std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>>& UniformBufferList);
		void SetStorageBufferList(const std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>>& StorageBufferList);
		void SetTextureBufferList(const std::vector<graphics::STextureBindingLayout>& TextureBufferList);

		std::shared_ptr<CMaterial> CreateMaterial(api::IGraphicsAPI* pGraphicsAPI, int RefCount);
	};
}