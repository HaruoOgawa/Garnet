#include "C3DObjectLoader.h"
#include "CResourceManager.h"
#include "../Message/Console.h"
#include "../Graphics/CVertexBuffer.h"
#include "../Format/CPathFormatter.h"
#include "../LoadWorker/CLoadWorker.h"
#include "../GLTF/CGLTFImporter.h"

#if defined(USE_FBX)
#ifdef USE_SMALL_FBX
#include "../FBX/CSmallFBXImporter.h"
#else
#include "../FBX/CFBXImporter.h"
#endif // USE_SMALL_FBX
#endif

#ifdef USE_MMD
#include "../MMD/PMX/CPmxImporter.h"
#endif

namespace resource
{
	C3DObjectLoader::C3DObjectLoader(const std::string& FileName, const std::shared_ptr<object::C3DObject>& TargetObject,
		const std::vector<std::shared_ptr<graphics::CMaterialFrame>>& BaseMaterialFrameList, const std::vector<std::string>& defaultmaterialframes,
		animation::ERigType RigType, const std::map<animation::EHumanoidBones, std::string>& HumanoidBoneList, int InstanceCount) :
		CResource(FileName, 3),
		m_LoadState(E3DObjectLoadState::None),
		m_TargetObject(TargetObject),
		m_TargetRigType(RigType),
		m_TargetHumanoidBoneList(HumanoidBoneList),
		m_BaseMaterialFrameList(BaseMaterialFrameList),
		m_InstanceCount(InstanceCount)
	{
		m_TargetObject->SetFileName(FileName);

		for (const auto& Name : defaultmaterialframes)
		{
			m_TargetObject->AddDefaultMaterialFrame(Name);
		}
	}

	C3DObjectLoader::~C3DObjectLoader()
	{
	}

	bool C3DObjectLoader::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
			return true;
		}
		
		switch (m_LoadState)
		{
		case resource::E3DObjectLoadState::None:
			m_LoadState = resource::E3DObjectLoadState::ImportObject;
			return true;
		case resource::E3DObjectLoadState::ImportObject:
			{
				if (!Import(pGraphicsAPI, pPhysicsEngine)) return false;
				
				m_LoadState = resource::E3DObjectLoadState::LoadSubResouce;

				return true;
			}
		case resource::E3DObjectLoadState::LoadSubResouce:
			{
				if (!LoadSubResources(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;

				if (static_cast<int>(m_SubResources.size()) == 0) m_LoadState = resource::E3DObjectLoadState::Finish;

				return true;
			}
		case resource::E3DObjectLoadState::Finish:
			break;
		default:
			break;
		}

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		// リソースマネージャーに登録
		pLoadWorker->GetResourceManager()->AddOnMemoryResource(shared_from_this(), nullptr);

		// インスタンス描画数を設定
		for (const auto& Mesh : m_TargetObject->GetMeshList())
		{
			for (const auto& VertexBuffer : Mesh->GetVertexBufferList())
			{
				VertexBuffer->SetInstanceDrawCount(m_InstanceCount);
			}
		}

		// CSceneControllerで生成するのでここでは無視(C3DObjectLoader・CSceneController)
		// Object生成
		//if (!m_TargetObject->Create(pGraphicsAPI, pPhysicsEngine)) return false;

		pApp->OnObjectLoaded(m_TargetObject, pGraphicsAPI, pLoadWorker);

		return true;
	}

	void C3DObjectLoader::AddSubResource(const std::shared_ptr <resource::IResource>& Resource)
	{
		m_SubResources.push_back(Resource);
	}

	bool C3DObjectLoader::Import(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		const auto& Extention = m_File->GetExtention();

		if (Extention == "gltf")
		{
#ifdef USE_GLTF
			std::string BaseDir = format::CPathFormatter::GetParentDir(m_FileName);
			if (!gltf::CGLTFImporter::ImportFromString(pGraphicsAPI, m_File->GetData(), BaseDir, m_TargetObject.get(), m_BaseMaterialFrameList, this, m_TargetRigType, m_TargetHumanoidBoneList)) return false;
#endif
		}
		else if (Extention == "glb")
		{
#ifdef USE_GLTF
			if (!gltf::CGLTFImporter::ImportFromMemory(pGraphicsAPI, m_File->GetData(), m_TargetObject.get(), m_BaseMaterialFrameList, this, m_TargetRigType, m_TargetHumanoidBoneList)) return false;
#endif
		}
		else if (Extention == "fbx")
		{
#if defined(USE_FBX)
#ifdef USE_SMALL_FBX
			if (!fbx::CSmallFBXImporter::ImportFBX(pGraphicsAPI, m_File->GetData(), m_TargetObject.get(), m_BaseMaterialFrameList, this, m_TargetRigType, m_TargetHumanoidBoneList)) return false;
#else
			if (!fbx::CFBXImporter::ImportFBX(pGraphicsAPI, m_FileName, m_TargetObject.get(), m_BaseMaterialFrameList, this, m_TargetRigType, m_TargetHumanoidBoneList)) return false;
#endif // USE_SMALL_FBX
#endif
		}
		else if (Extention == "pmx")
		{
#ifdef USE_MMD
			if (!mmd::CPmxImporter::ImportPmx(pGraphicsAPI, pPhysicsEngine, m_FileName, m_File->GetData(), m_TargetObject.get(), m_BaseMaterialFrameList, this, m_TargetRigType, m_TargetHumanoidBoneList)) return false;
#endif
		}
		else
		{
			Console::Log("[Error - 3DObjectLoader] Unsupported file extention: %s\n", Extention.c_str());

			return false;
		}

		return true;
	}

	bool C3DObjectLoader::LoadSubResources(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, app::CApp* pApp)
	{
		// サブリソースのロード
		for (auto& Resource : m_SubResources)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				// ファイルのバイナリが実行ファイルに埋め込まれていないかチェックする
				if (pLoadWorker->FindEmbeddedBinary(pGraphicsAPI, pPhysicsEngine, Resource, pApp)) return true;

				// 通常通りロードする
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, pLoadWorker, pApp)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				// リソースマネージャーに登録
				pLoadWorker->GetResourceManager()->AddOnMemoryResource(Resource, shared_from_this());

				m_SubResources.erase(m_SubResources.begin());
				m_SubResources.shrink_to_fit();
			}
			return true;

			default:
				break;
			}
		}

		return true;
	}
}