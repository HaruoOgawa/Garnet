#include "CSceneLoader.h"
#include "CLoadWorker.h"
#include "CMaterialFrameLoader.h"
#include "../Scene/CSceneController.h"
#include "../Object/C3DObject.h"

namespace resource
{
	CSceneLoader::CSceneLoader(const std::string& FileName, const std::shared_ptr<scene::CSceneController>& Target):
		CResource(FileName),
		m_Target(Target)
	{
	}

	CSceneLoader::~CSceneLoader()
	{
	}

	bool CSceneLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_File->IsLoaded()) return true;

		// シーン読み込み
		if (!AnalyseScene(pLoadWorker)) return false;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}

	bool CSceneLoader::AnalyseScene(resource::CLoadWorker* pLoadWorker)
	{
		std::string RawData = std::string();
		RawData.resize(m_File->GetData().size());
		std::memcpy(&RawData[0], &m_File->GetData()[0], m_File->GetData().size());

		json SceneJSON = json::parse(RawData.c_str());

		// materialframes
		{
			const auto materialframes = SceneJSON.find("materialframes");
			if (materialframes != SceneJSON.end() && materialframes->is_array())
			{
				if (!AnalyseMaterialFrames(materialframes, pLoadWorker)) return false;
			}
		}

		// objects
		{
			const auto objects = SceneJSON.find("objects");
			if (objects != SceneJSON.end() && objects->is_array())
			{
				if (!AnalyseObjects(objects, pLoadWorker)) return false;
			}
		}

		return true;
	}

	bool CSceneLoader::AnalyseMaterialFrames(const json::iterator& materialframes, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator mfJSON = materialframes->begin(); mfJSON != materialframes->end(); mfJSON++)
		{
			if (!mfJSON->is_object()) continue;

			std::string name = "";
			GetString("name", name, mfJSON);

			std::string filename = "";
			GetString("filename", filename, mfJSON);

			if (name.empty() || filename.empty()) continue;

			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();

			m_MaterialFrameMap.emplace(filename, MaterialFrame);
			pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame));
		}

		return true;
	}

	bool CSceneLoader::AnalyseObjects(const json::iterator& objects, resource::CLoadWorker* pLoadWorker)
	{
		for (json::iterator objectJSON = objects->begin(); objectJSON != objects->end(); objectJSON++)
		{
			if (!objectJSON->is_object()) continue;

			std::shared_ptr<object::C3DObject> Object = std::make_shared<object::C3DObject>("", "");

			// ObjectName
			{
				std::string objname = "";
				GetString("name", objname, objectJSON);

				Object->SetObjectName(objname);
			}

			// Transform
			{
				auto Transform = AnalyseTransform(objectJSON);

				Object->SetPos(Transform->GetPos());
				Object->SetRot(Transform->GetRot());
				Object->SetScale(Transform->GetScale());
			}

			// rootnodes
			{
				// ToDo: この2次元配列のIndexListは害悪なのでリファクタリングしたい
				std::vector<std::vector<int>> RootNodeIndexList;

				std::vector<int> rootnodes;

				GetArrayInt32("rootnodes", rootnodes, objectJSON);

				for (int root : rootnodes)
				{
					RootNodeIndexList.push_back(std::vector<int>({ root }));
				}

				Object->SetRootNodeIndexList(RootNodeIndexList);
			}

			// nodes
			const auto nodes = objectJSON->find("nodes");
			if (nodes != objectJSON->end() && nodes->is_array())
			{
				for (json::iterator nodeJSON = nodes->begin(); nodeJSON != nodes->end(); nodeJSON++)
				{
					if (!nodeJSON->is_object()) continue;

					std::string nodename = "";
					GetString("name", nodename, nodeJSON);

					auto Transform = AnalyseTransform(nodeJSON);

					int meshindex = -1;
					GetInt("meshindex", meshindex, nodeJSON);

					std::vector<int> children;
					GetArrayInt32("children", children, nodeJSON);

					// ノードを作成
					int SelfNodeIndex = static_cast<int>(Object->GetNodeList().size());

					std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(meshindex, SelfNodeIndex);

					Node->SetName(nodename);
					Node->SetLocalTransform(Transform);
					Node->SetChildrenNodeIndexList(children);
					
					// 登録
					Object->AddNode(Node);
				}
			}

			// Objectを追加
			m_Target->AddObject(Object);
		}

		return true;
	}

	std::shared_ptr<math::CTransform> CSceneLoader::AnalyseTransform(const json::iterator& Object)
	{
		std::shared_ptr<math::CTransform> Transform = std::make_shared<math::CTransform>();

		const auto transformJSON = Object->find("transform");
		if (transformJSON->is_object())
		{
			// pos
			glm::vec3 pos = glm::vec3(0.0f);
			{
				std::vector<float> posArray;
				GetArrayFloat32("pos", posArray, transformJSON);

				if (posArray.size() == 3)
				{
					pos = glm::vec3(posArray[0], posArray[1], posArray[2]);
				}
			}

			// rotate
			glm::quat rotate = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			{
				std::vector<float> rotateArray;
				GetArrayFloat32("rotate", rotateArray, transformJSON);

				if (rotateArray.size() == 3)
				{
					// rotateArrayはDegreeである
					rotate =
						glm::angleAxis(glm::radians(rotateArray[2]), glm::vec3(0.0f, 0.0f, 1.0f)) *
						glm::angleAxis(glm::radians(rotateArray[1]), glm::vec3(0.0f, 1.0f, 0.0f)) *
						glm::angleAxis(glm::radians(rotateArray[0]), glm::vec3(1.0f, 0.0f, 0.0f));
				}
			}

			// scale
			glm::vec3 scale = glm::vec3(1.0f);
			{
				std::vector<float> scaleArray;
				GetArrayFloat32("scale", scaleArray, transformJSON);

				if (scaleArray.size() == 3)
				{
					scale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
				}
			}

			// 登録
			Transform->SetPos(pos);
			Transform->SetRot(rotate);
			Transform->SetScale(scale);
		}

		return Transform;
	}

	void CSceneLoader::GetString(const std::string& Key, std::string& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_string())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetBoolean(const std::string& Key, bool& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_boolean())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetInt(const std::string& Key, int& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_integer())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetArrayInt32(const std::string& Key, std::vector<int>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					int val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}

	void CSceneLoader::GetFloat(const std::string& Key, float& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_number_float())
		{
			Value = it.value();
		}
	}

	void CSceneLoader::GetArrayFloat32(const std::string& Key, std::vector<float>& Value, const json::iterator& Object)
	{
		const auto it = Object->find(Key);
		if (it != Object->end() && it->is_array())
		{
			for (json::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				if (it2->is_number())
				{
					float val = it2.value();

					Value.push_back(val);
				}
			}
		}
	}
}