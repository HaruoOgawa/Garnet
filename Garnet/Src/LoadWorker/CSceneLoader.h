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

namespace scene { class CSceneController; }
namespace object { class C3DObject; }
namespace graphics { class CMaterialFrame; }

namespace resource
{
	class CSceneLoader : public resource::CResource
	{
		std::shared_ptr<scene::CSceneController> m_Target;

		std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>> m_MaterialFrameMap;
	private:
		bool AnalyseScene(resource::CLoadWorker* pLoadWorker);
		bool AnalyseMaterialFrames(const json::iterator& materialframes, resource::CLoadWorker* pLoadWorker);
		bool AnalyseObjects(const json::iterator& objects, resource::CLoadWorker* pLoadWorker);

		std::shared_ptr<math::CTransform> AnalyseTransform(const json::iterator& Object);

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