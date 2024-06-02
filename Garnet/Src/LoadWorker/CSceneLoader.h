#pragma once

#include <memory>
#include <string>
#include <map>
#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CResource.h"
#include "../Math/CTransform.h"

using namespace nlohmann;

namespace scene { 
	class CSceneController; 
	struct SMaterialInfo;
	struct SAnimationInfo;
}
namespace object { 
	class C3DObject; 
	class CNode;
}
namespace graphics { 
	class CMesh;
	class CTextureSet;
}

namespace resource
{
	class CSceneLoader : public resource::CResource
	{
		std::shared_ptr<scene::CSceneController> m_Target;
	private:
		bool AnalyseScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		bool AnalyseMaterialFrames(const json::iterator& materialframes, resource::CLoadWorker* pLoadWorker);
		bool AnalyseSceneTextureSet(const json::iterator& scenetexturesetJSON, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, std::shared_ptr<graphics::CTextureSet>& SceneTextureSet);
		bool AnalyseSceneAnimations(const json::iterator& animations, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);
		bool AnalyseObjects(const json::iterator& objects, api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		std::shared_ptr<math::CTransform> AnalyseTransform(const json::iterator& Object);
		std::shared_ptr<object::CNode> AnalyseNode(const json::iterator& nodeJSON, const std::shared_ptr<object::C3DObject>& Object);
		std::shared_ptr<graphics::CMesh> AnalyseMesh(api::IGraphicsAPI* pGraphicsAPI, const json::iterator& meshJSON, std::map<int, int>& MatRefCountMap);
		scene::SMaterialInfo AnalyseMaterialInfo(const json::iterator& materialJSON, int MaterialIndex, const std::map<int, int>& MatRefCountMap);
		scene::SAnimationInfo AnalyseAnimationInfo(const json::iterator& animationJSON);

		void GetString(const std::string& Key, std::string& Value, const json::iterator& Object);
		void GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object);
		void GetInt(const std::string& Key, int& Value, const json::iterator& Object);
		void GetArrayInt32(const std::string& Key, std::vector<int>& Value, const json::iterator& Object);
		void GetFloat(const std::string& Key, float& Value, const json::iterator& Object);
		void GetArrayFloat32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object);
	public:
		CSceneLoader(const std::string& FileName, const std::shared_ptr<scene::CSceneController>& Target);
		virtual ~CSceneLoader();

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker) override;
	};
}