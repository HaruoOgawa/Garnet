#pragma once

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>
#include <json.hpp>

#include "CResource.h"

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
	class CShaderLoader;

	class CMaterialFrameLoader : public resource::CResource
	{
		json m_MfJson;

		// MfResource
		bool m_AnalyseDone;
		std::vector<std::shared_ptr<IResource>> m_MfResourceList;

		std::unordered_map <std::string, CShaderLoader*> m_ShaderFileList;
		std::unordered_map <std::string, std::shared_ptr<IResource>> m_TextureFileList;

		// MfData
		std::string m_MaterialName;

		std::vector<graphics::SShaderBufferSet> m_ShaderBufferList;
		std::vector<graphics::STextureBindingLayout> m_TextureBufferList;

		std::shared_ptr<graphics::CMaterialCreateInfo> m_CreateInfo;

		std::set<std::shared_ptr<graphics::CMaterialFrame>> m_TargetMaterialFrameSet;
	private:
		bool AnalyseResourceList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager);
		bool AnalyseShaderList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager, const json::iterator& shaderList);
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
		CMaterialFrameLoader(const std::string& filename, const std::shared_ptr<graphics::CMaterialFrame>& TargetMaterialFrame);
		virtual ~CMaterialFrameLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager) override;

		virtual void AddReference(const std::shared_ptr<IResource>& Resource) override;

		const std::set<std::shared_ptr<graphics::CMaterialFrame>>& GetTargetMaterialFrameSet() const;
	};
}