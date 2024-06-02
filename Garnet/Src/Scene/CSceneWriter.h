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
		static bool WriteScene(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteMaterialFrames(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSceneTextureSet(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteAnimations(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSound(json& SceneJSON, CSceneController* pSceneController);
		static bool WriteObjects(json& SceneJSON, CSceneController* pSceneController);

		static bool WriteNodes(json& ObjectJSON, object::C3DObject* pObject);
		static bool WriteMeshs(json& ObjectJSON, object::C3DObject* pObject);
		static bool WriteMaterials(json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		static bool WriteTextureSet(json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		static bool WriteAnimation(json& ObjectJSON, object::C3DObject* pObject, const SAnimationInfo& AnimationInfo);
	public:
		static bool Write(CSceneController* pSceneController);
	};
}