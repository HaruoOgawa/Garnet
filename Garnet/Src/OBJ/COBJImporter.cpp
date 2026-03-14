#include "COBJImporter.h"
#include "../../Interface/IGraphicsAPI.h"
#include "../../Object/C3DObject.h"
#include "../../Graphics/CMaterialFrame.h"
#include "../../LoadWorker/C3DObjectLoader.h"
#include "../../Binary/CBinaryReader.h"
#include "../../Format/CStringFormatter.h"
#ifdef _DEBUG
#include "../../Message/Console.h"
#endif // _DEBUG

namespace obj
{
	bool COBJImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
		const std::string& BaseDir, object::C3DObject* Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, resource::C3DObjectLoader* p3DObjectLoader)
	{
		binary::CBinaryReader Analyzer(Data);

		std::vector<std::string> LineList;
		std::vector<float> Vertices;  // Dimention: 3
		std::vector<float> Texcoords; // Dimention: 2
		std::vector<float> Normals; // Dimention: 3

		for (;;)
		{
			if (Analyzer.IsEnd()) break;

			std::string CurrentLine = std::string();
			if (!Analyzer.GetStringToLineBreak(CurrentLine)) return false;
			LineList.push_back(CurrentLine);

			std::vector<std::string> ParamList = format::CStringFormatter::Sprint(CurrentLine, ' ');
			if (ParamList.empty()) continue;

			Analyze(ParamList, CurrentLine, Vertices, Texcoords, Normals);
		}

		return true;
	}

	void COBJImporter::Analyze(const std::vector<std::string>& ParamList, const std::string& CurrentLine,
		std::vector<float>& Vertices, std::vector<float>& Texcoords, std::vector<float>& Normals)
	{
		if (ParamList[0] == "v" && ParamList.size() >= 4)
		{
			// position
			Vertices.push_back(std::stof(ParamList[1]));
			Vertices.push_back(std::stof(ParamList[2]));
			Vertices.push_back(std::stof(ParamList[3]));
		}
		else if (ParamList[0] == "vt" && ParamList.size() >= 4)
		{
			// uv
			Texcoords.push_back(std::stof(ParamList[1]));
			Texcoords.push_back(std::stof(ParamList[2]));
		}
		else if (ParamList[0] == "vn" && ParamList.size() >= 4)
		{
			// normal
			Normals.push_back(std::stof(ParamList[1]));
			Normals.push_back(std::stof(ParamList[2]));
			Normals.push_back(std::stof(ParamList[3]));
		}
		else if (ParamList[0] == "f" && ParamList.size() >= 4)
		{
			// face(頂点インデックス)
		}
		else if (ParamList[0] == "usemtl" && ParamList.size() >= 2)
		{
			// プリミティブ名(ここでプリミティブを分割する)
			std::string PrimitiveName = ParamList[1];
		}
		else
		{
#ifdef _DEBUG
			Console::Log("[COBJImporter::Analyze] Non Supported Type Found. (%s)\n", CurrentLine.c_str());
#endif // _DEBUG
		}
	}
}