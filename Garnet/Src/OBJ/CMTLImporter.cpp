#include "CMTLImporter.h"
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
	bool CMTLImporter::Import(api::IGraphicsAPI* pGraphicsAPI, const std::vector<unsigned char>& Data,
		const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList,
		resource::C3DObjectLoader* p3DObjectLoader)
	{
		binary::CBinaryReader Analyzer(Data);

		std::vector<std::string> LineList;

		for (;;)
		{
			if (Analyzer.IsEnd()) break;

			std::string CurrentLine = std::string();
			if (!Analyzer.GetStringToLineBreak(CurrentLine)) return false;
			LineList.push_back(CurrentLine);

			std::vector<std::string> ParamList = format::CStringFormatter::Sprit(CurrentLine, ' ');
			if (ParamList.empty()) continue;

			Analyze(ParamList, CurrentLine, BaseDir, Object, p3DObjectLoader);
		}

		return true;
	}

	void CMTLImporter::Analyze(const std::vector<std::string>& ParamList, const std::string& CurrentLine,
		const std::string& BaseDir, const std::shared_ptr<object::C3DObject>& Object, resource::C3DObjectLoader* p3DObjectLoader)
	{
		if (ParamList[0] == "newmtl" && ParamList.size() >= 2)
		{
			std::string PrimitiveName = ParamList[1];
		}
		else if (ParamList[0] == "#")
		{
			// ÉRÉÅÉìÉgÇ»ÇÃÇ≈âΩÇ‡ÇµÇ»Ç¢
			return;
		}
		else
		{
#ifdef _DEBUG
			Console::Log("[COBJImporter::Analyze] Non Supported Type Found. (%s)\n", CurrentLine.c_str());
#endif // _DEBUG
		}
	}
}