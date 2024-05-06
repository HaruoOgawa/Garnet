#pragma once
#ifdef USE_TEXTURE_LOADER

#include <vector>
#include <string>
#include <memory>

#include "../Interface/IResource.h"

#include "CFile.h"
#include "../Graphics/CTexture.h"

namespace resource
{
	class CTextureLoader : public resource::IResource
	{
		resource::ELoadStatus m_Status;

		std::vector<std::shared_ptr<CFile>> m_FileList;
		std::string m_FileName;

		std::vector<unsigned char> m_TextureData;

		bool m_UseCube;
		std::vector<std::vector<unsigned char>> m_CubeDataList;

		std::shared_ptr<graphics::CTexture> m_TargetTexture;
	public:
		CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::vector<std::string>& FileNameList, const std::shared_ptr<graphics::CTexture>& Texture);
		CTextureLoader(api::IGraphicsAPI* pGraphicsAPI, const std::string& FileName, const std::shared_ptr<graphics::CTexture>& Texture);
		virtual ~CTextureLoader();

		virtual const std::string& GetFilename() const override;

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool LoadImmediate() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}

#endif