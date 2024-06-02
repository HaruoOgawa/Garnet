#include "CSceneWriter.h"
#include "CSceneController.h"
#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Audio/CAudioClip.h"
#include "../LoadWorker/CFile.h"

namespace scene
{
	bool CSceneWriter::Write(CSceneController* pSceneController)
	{
		json SceneJSON;

		// シーンJSONに現在の状態を書き出し
		if (!WriteScene(SceneJSON, pSceneController)) return false;

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

	bool CSceneWriter::WriteScene(json& SceneJSON, CSceneController* pSceneController)
	{
		if (!WriteMaterialFrames(SceneJSON, pSceneController)) return false;
		if (!WriteSceneTextureSet(SceneJSON, pSceneController)) return false;
		if (!WriteAnimations(SceneJSON, pSceneController)) return false;
		if (!WriteSound(SceneJSON, pSceneController)) return false;

		return true;
	}

	bool CSceneWriter::WriteMaterialFrames(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& MaterialFrameMap = pSceneController->GetMaterialFrameMap();

		for (const auto& MaterialFrame : MaterialFrameMap)
		{
			SceneJSON["materialframes"].push_back({ {"name" , MaterialFrame.first}, { "filename", MaterialFrame.second->GetFileName()} });
		}

		return true;
	}

	bool CSceneWriter::WriteSceneTextureSet(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& SceneTextureSet = pSceneController->GetSceneTextureSet();

		for (const auto& CubeMap : SceneTextureSet->GetCubeMapList())
		{
			const auto& FileNameList = CubeMap->GetFileNameList();
			if (FileNameList.size() != 6) continue;

			SceneJSON["scenetextureset"]["cubemaps"].push_back({
				{"back", FileNameList[0]},
				{"bottom", FileNameList[1]},
				{"front", FileNameList[2]},
				{"left", FileNameList[3]},
				{"right", FileNameList[4]},
				{"top", FileNameList[5]}
			});
		}

		SceneJSON["scenetextureset"]["frametextures"] = {};

		const auto& Diffuse_Tex = SceneTextureSet->GetDiffuse_Tex();
		const auto& Specular_Tex = SceneTextureSet->GetSpecular_Tex();
		const auto& GGXLUT_Tex = SceneTextureSet->GetGGXLUT_Tex();
		if (Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
		{
			SceneJSON["scenetextureset"]["ibl"] = {
				{"diffuse", Diffuse_Tex->GetFileName()},
				{"specular", Specular_Tex->GetFileName()},
				{"ggx", GGXLUT_Tex->GetFileName()}
			};
		}

		return true;
	}

	bool CSceneWriter::WriteAnimations(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& AnimationClipSetMap = pSceneController->GetAnimationClipSetMap();

		for (const auto& AnimationClipSet : AnimationClipSetMap)
		{
			SceneJSON["animations"].push_back({ {"name" , AnimationClipSet.first}, { "filename", AnimationClipSet.second->GetFileName()} });
		}

		return true;
	}

	bool CSceneWriter::WriteSound(json& SceneJSON, CSceneController* pSceneController)
	{
		const auto& Sound = pSceneController->GetSound();

		const auto& AudioClip = std::get<0>(Sound);
		bool autoplay = std::get<1>(Sound);
		bool loop = std::get<2>(Sound);
		
		if (AudioClip)
		{
			SceneJSON["sound"]["bgm"] = {
				{ "filename", AudioClip->GetFileName() },
				{ "autoplay", autoplay },
				{ "loop", loop }
			};
		}

		return true;
	}
}