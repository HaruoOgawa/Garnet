#pragma once

#include <vector>
#include <json.hpp>
using namespace nlohmann;

namespace object { class C3DObject; }
namespace graphics { class CTexture; }

namespace scene
{
	class CSceneController;
	struct SLoadTextureInfo;
	struct SAnimationInfo;

	class CSceneWriter
	{
	private:
		static bool WriteScene(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteMaterialFrames(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSceneTextureSet(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteAnimations(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSound(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteObjects(ordered_json& SceneJSON, CSceneController* pSceneController);

		static bool WriteNodes(ordered_json& ObjectJSON, object::C3DObject* pObject);
		static bool WriteMeshs(ordered_json& ObjectJSON, object::C3DObject* pObject);
		static bool WriteMaterials(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		static bool WriteTextureSet(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		static bool WriteAnimation(ordered_json& ObjectJSON, object::C3DObject* pObject, const SAnimationInfo& AnimationInfo);
	public:
		static bool Write(CSceneController* pSceneController);
	};
}