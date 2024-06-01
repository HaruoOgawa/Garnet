#pragma once

#include <string>
#include <memory>
#include <set>

#include "CResource.h"

#include "CFile.h"

namespace resource
{
	class CMaterialFrameLoader;

	class CShaderLoader : public CResource
	{
		std::string m_EditingBaseFileName;

		std::set<std::shared_ptr<IResource>> m_RefMFLoaderSet;
	private:
		std::vector<std::string> ExecuteCommand(const char* cmd);

		std::vector<std::string> Split(const std::string& Src, const char separate);
	public:
		CShaderLoader(const std::string& FileName, const std::string& EditingBaseFileName);
		virtual ~CShaderLoader();

		virtual const std::string& GetFilename() const override;
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;

		const std::shared_ptr<CFile>& GetFile() const;

		virtual bool Reload(resource::CLoadWorker* pLoadWorker) override;

		void AddRefMFLoader(const std::shared_ptr<IResource>& MFLoader);
	};
}