#pragma once

#include <json.hpp>
using namespace nlohmann;

namespace scene
{
	class CSceneController;

	class CSceneWriter
	{
	private:
		static bool WriteScene(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteMaterialFrames(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSceneTextureSet(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteAnimations(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSound(json& SceneJSON, CSceneController* pSceneController);
	public:
		static bool Write(CSceneController* pSceneController);
	};
}