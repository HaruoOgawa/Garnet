#include "CShaderLoader.h"
#include "CResourceManager.h"
#include "../Message/Console.h"

namespace resource
{
	CShaderLoader::CShaderLoader(const std::string& FileName, const std::string& BaseFileName, const std::string& shaderType, bool autoShaderExtension):
		CResource(FileName, 1),
		m_EditingBaseFileName(BaseFileName)
	{
		if (autoShaderExtension)
		{
			if (shaderType == "vertex")
			{
				m_EditingBaseFileName += ".vert";
			}
			else if (shaderType == "fragment")
			{
				m_EditingBaseFileName += ".frag";
			}
			else if (shaderType == "compute")
			{
				m_EditingBaseFileName += ".comp";
			}
			else if (shaderType == "geometry")
			{
				m_EditingBaseFileName += ".geom";
			}
			else if (shaderType == "hull")
			{
				m_EditingBaseFileName += ".tesc";
			}
			else if (shaderType == "domain")
			{
				m_EditingBaseFileName += ".tese";
			}
		}
	}

	CShaderLoader::~CShaderLoader()
	{
	}
	
	const std::string& CShaderLoader::GetFilename() const
	{
		return m_EditingBaseFileName;
	}

	bool CShaderLoader::Update(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<CResourceManager>& ResourceManager)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, ResourceManager)) return false;
			return true;
		}

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		ResourceManager->AddOnMemoryResource(shared_from_this(), nullptr);

		return true;
	}

	const std::shared_ptr<CFile>& CShaderLoader::GetFile() const
	{
		return m_File;
	}

	bool CShaderLoader::Reload(resource::CLoadWorker* pLoadWorker)
	{
		{
			std::string cmd = "call Commands/CompileGLSLToSpirV.bat " + m_EditingBaseFileName;
			std::vector<std::string> Result = ExecuteCommand(cmd.c_str());

			for (const auto& cmdLog : Result)
			{
				Console::Log("[Shader Reload] cmdLog: %s\n", cmdLog.c_str());
			}
		}

		{
			std::string cmd = "call Commands/CompileSpirVToWGSL.bat " + m_EditingBaseFileName;
			std::vector<std::string> Result = ExecuteCommand(cmd.c_str());

			for (const auto& cmdLog : Result)
			{
				Console::Log("[Shader Reload] cmdLog: %s\n", cmdLog.c_str());
			}
		}

		return true;
	}

	std::vector<std::string> CShaderLoader::ExecuteCommand(const char* cmd)
	{
#ifdef __EMSCRIPTEN__
		return std::vector<std::string>();
#else
		char buffer[128];
		std::string result = "";

		FILE* pipe = _popen(cmd, "r");
		if (!pipe)
		{
			Console::Log("[Error] Failed to execute command\n");
			return std::vector<std::string>();
		}

		while (fgets(buffer, sizeof buffer, pipe) != NULL)
		{
			result += buffer;
		}

		_pclose(pipe);

		return Split(result, '\n');
#endif // __EMSCRIPTEN__

	}

	std::vector<std::string> CShaderLoader::Split(const std::string& Src, const char separate)
	{
		std::vector<std::string> TextList;

		std::string CurrentStr = Src;

		while (CurrentStr.find(separate) != -1)
		{
			int Index = static_cast<int>(CurrentStr.find(separate));

			std::string Dst = CurrentStr.substr(0, Index + 1);
			TextList.push_back(Dst);

			CurrentStr = CurrentStr.substr(Index + 1, CurrentStr.size() - Dst.size());
		}

		return TextList;
	}
}