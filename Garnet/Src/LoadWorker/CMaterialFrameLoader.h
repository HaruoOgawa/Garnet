#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <json.hpp>

#include "../Interface/IResource.h"

#include "CFile.h"

#include "../Graphics/CMaterialFrame.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

namespace graphics { 
	class CMaterialFrame; 
	class CMaterialCreateInfo;
}

using namespace nlohmann;

namespace resource
{
	class CFile;

	class CMaterialFrameLoader : public resource::IResource
	{
		//
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_MfFile;
		json m_MfJson;

		//
		bool m_AnalyseDone;
		std::vector<std::shared_ptr<CFile>> m_MfResourceList;

		std::unordered_map <std::string, std::shared_ptr<CFile>> m_ShaderFileList;
		std::unordered_map <std::string, std::shared_ptr<CFile>> m_TextureFileList;

		//
		std::shared_ptr<graphics::CMaterialFrame> m_TargetMaterialFrame;
		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;
	private:
		bool AnalyseResourceList(api::IGraphicsAPI* pGraphicsAPI);
		bool AnalyseShaderList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& shaderList);
		bool AnalyseTextureList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& textureList);

		bool CreateMaterialFrame(api::IGraphicsAPI* pGraphicsAPI);
	public:
		CMaterialFrameLoader(const std::string& filename, std::shared_ptr<graphics::CMaterialFrame> TargetMaterialFrame);
		virtual ~CMaterialFrameLoader();

		virtual void SetLoadStatus(resource::ELoadStatus Status) override;
		virtual resource::ELoadStatus GetStatus() const override;
		virtual bool IsLoaded() const override;

		virtual bool Load() override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI) override;
	};
}