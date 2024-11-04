#pragma once
#ifdef USE_BINARY_WRITE
#include <vector>
#include <set>
#include <json.hpp>
using namespace nlohmann;

namespace object { class C3DObject; }
namespace graphics { class CTexture; }
namespace timeline { class CTimelineController; }
namespace animation { class CSkeleton; }

namespace scene
{
	class CSceneController;
	struct SLoadTextureInfo;
	struct SAnimationInfo;

	class CSceneWriter
	{
	private:
		static bool WriteScene(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		static bool WriteMaterialFrames(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteValueRegistries(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		static bool WriteSceneTextureSet(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteAnimations(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteSound(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteTimeline(ordered_json& SceneJSON, CSceneController* pSceneController);
		static bool WriteObjects(ordered_json& SceneJSON, CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController);

		static bool WriteNodes(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		static bool WriteMeshs(ordered_json& ObjectJSON, object::C3DObject* pObject);
		static bool WriteMaterials(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList, const std::shared_ptr<timeline::CTimelineController>& TimelineController);
		static bool WriteTextureSet(ordered_json& ObjectJSON, object::C3DObject* pObject, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList);
		static bool WriteAnimation(ordered_json& ObjectJSON, object::C3DObject* pObject, const SAnimationInfo& AnimationInfo);

		static bool WriteHumanoidBoneList(json& AnimationJSON, const std::shared_ptr<animation::CSkeleton>& Skeleton);
	public:
		static bool Write(CSceneController* pSceneController, const std::shared_ptr<timeline::CTimelineController>& TimelineController);
	};
}
#endif // USE_BINARY_WRITE