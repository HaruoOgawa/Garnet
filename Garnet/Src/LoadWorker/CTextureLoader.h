#pragma once
#ifdef USE_TEXTURE_LOADER

#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"
#include "CResource.h"

#include "CFile.h"
#include "../Graphics/CTexture.h"

namespace resource
{
	class CTextureLoader : public resource::CResource
	{
		std::vector<std::shared_ptr<IResource>> m_FileList;

		std::vector<unsigned char> m_TextureData;

		bool m_UseCube;
		std::vector<std::vector<unsigned char>> m_CubeDataList;

		std::shared_ptr<graphics::CTexture> m_TargetTexture;
	public:
		CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::string>& FileNameList, const std::shared_ptr<graphics::CTexture>& Texture);
		CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, const std::shared_ptr<graphics::CTexture>& Texture);
		virtual ~CTextureLoader();

		virtual bool Load() override;
		virtual bool LoadImmediate() override;

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp) override;
	};
}

#endif