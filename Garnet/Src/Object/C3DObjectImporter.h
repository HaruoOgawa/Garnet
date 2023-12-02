#pragma once

#include "C3DObject.h"

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "../Interface/IGraphicsAPI.h"

namespace graphics
{
	class CTexture;
	class CTextureSet;
	class CMaterial;
	class CMesh;
	class CMaterialCreateInfo;
	class CMaterialFrame;
}

namespace object
{
	class C3DObjectImporter
	{
	public:
		static bool ImportSimply(api::IGraphicsAPI* pGraphicsAPI, std::shared_ptr<object::C3DObject>& Object,
			const std::shared_ptr<renderer::CRendererCreateInfo>& createInfo,
			const std::shared_ptr<graphics::CMaterial>& Material, const std::shared_ptr<graphics::CTextureSet>& TextureSet,
			const std::shared_ptr<resource::CFile>& DepthVertex, const std::shared_ptr<resource::CFile>& DepthFragment);
	};
}