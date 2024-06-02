#include "CSceneWriter.h"
#include "CSceneController.h"
#include "../LoadWorker/CFile.h"

#include <json.hpp>
using namespace nlohmann;

namespace scene
{
	bool CSceneWriter::Write(CSceneController* pSceneController)
	{
		json SceneJSON;

		SceneJSON["test"] = "Hello Writer";
		SceneJSON["aaa"] = 12345;
		SceneJSON["www"] = false;
		SceneJSON["arrayTest"] = { 0.0f, 0.0f };
		SceneJSON["SubTest"]["SecondBlock"] = 123;
		SceneJSON["Object"] = { {"currency", "USD"}, {"value", 42.99} };

		// データを取得
		const auto& JSONStr = SceneJSON.dump(4);

		std::vector<unsigned char> Data;
		Data.resize(JSONStr.size());

		std::memcpy(&Data[0], &JSONStr[0], static_cast<int>(JSONStr.size()));

		// ファイル書き出し
		resource::CFile File = resource::CFile("Resources\\Scene\\SceneWriteTest.json");
		File.SetData(Data);
		if (!File.Write()) return false;

		return true;
	}
}