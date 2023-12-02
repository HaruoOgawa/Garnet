#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <json.hpp>

#include "../Interface/IResource.h"

#include "CFile.h"

#include "../Graphics/CMaterialFrame.h"
#include "../Graphics/SBindingLayout.h"
#include "../Graphics/SBufferValueLayout.h"
#include "../Graphics/STextureBindingLayout.h"
#include "../GraphicsAPI/CMaterialCreateInfo.h"

using namespace nlohmann;

namespace resource
{
	class CFile;

	class CMaterialFrameLoader : public resource::IResource
	{
		// MfStatus
		resource::ELoadStatus m_Status;

		std::shared_ptr<CFile> m_MfFile;
		json m_MfJson;

		// MfResource
		bool m_AnalyseDone;
		std::vector<std::shared_ptr<CFile>> m_MfResourceList;

		std::unordered_map <std::string, std::shared_ptr<CFile>> m_ShaderFileList;
		std::unordered_map <std::string, std::shared_ptr<CFile>> m_TextureFileList;

		// MfData
		std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>> m_UniformBufferList;
		std::vector<std::pair<graphics::SBindingLayout, std::vector<std::shared_ptr<graphics::SBufferValueLayout>>>> m_StorageBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;

		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::shared_ptr<graphics::CMaterialFrame> m_TargetMaterialFrame;
	private:
		bool AnalyseResourceList(api::IGraphicsAPI* pGraphicsAPI);
		bool AnalyseShaderList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& shaderList);
		bool AnalyseShaderBuffer(const json::iterator& uniform, const std::string& uniform_type);
		bool AnalyseTextureBuffer(const json::iterator& uniform);

		bool AnalyseTextureList(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& textureList);

		bool CreateMaterialFrame(api::IGraphicsAPI* pGraphicsAPI);

		void GetString(const std::string& Key, std::string& Value, const json::iterator& Object);
		void GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object);
		void GetInt(const std::string& Key, int& Value, const json::iterator& Object);
		void GetFloat(const std::string& Key, float& Value, const json::iterator& Object);
		void GetArray32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object);
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