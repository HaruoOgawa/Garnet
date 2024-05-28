#pragma once

#include <memory>
#include <string>
#include <json.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CResource.h"
#include "../Math/CTransform.h"

using namespace nlohmann;

namespace scene { class CSceneController; }
namespace object { class C3DObject; }

namespace resource
{
	class CSceneLoader : public resource::CResource
	{
		std::shared_ptr<scene::CSceneController> m_Target;
	private:
		bool AnalyseScene();
		bool AnalyseObjects(const json::iterator& objects);

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

		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, const std::shared_ptr<CResourceManager>& ResourceManager) override;
	};
}