#include "C3DObjectLoader.h"

#include "../GLTF/CGLTFImporter.h"
#include "../FBX/CFBXImporter.h"
#include "../Debug/Message/Console.h"

namespace resource
{
	C3DObjectLoader::C3DObjectLoader(const std::string& FileName, const  std::shared_ptr<object::C3DObject>& TargetObject, const std::string& PassName, const std::string& DepthPassName):
		m_Status(ELoadStatus::None),
		m_File(std::make_shared<CFile>(FileName)),
		m_FileName(FileName),
		m_TargetObject(TargetObject)
	{
	}

	C3DObjectLoader::~C3DObjectLoader()
	{
	}

	void C3DObjectLoader::SetLoadStatus(resource::ELoadStatus Status)
	{
		m_Status = Status;
	}

	resource::ELoadStatus C3DObjectLoader::GetStatus() const
	{
		return m_Status;
	}
	bool C3DObjectLoader::IsLoaded() const
	{
		return (m_Status == resource::ELoadStatus::Loaded);
	}

	bool C3DObjectLoader::Load()
	{
		m_Status = resource::ELoadStatus::Loading;

		if (!m_File->Load()) return false;

		return true;
	}

	bool C3DObjectLoader::LoadImmediate()
	{
		return true;
	}

	bool C3DObjectLoader::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_File->IsLoaded())
		{
			if (!m_File->Update(pGraphicsAPI)) return false;
			return true;
		}
		
		// 拡張子からオブジェクトタイプを判断する
		object::E3DObjectType ObjectType = object::E3DObjectType::None;

		const auto& Extention = m_File->GetExtention();
		if (Extention == "gltf")
		{
			ObjectType = object::E3DObjectType::glTF;
		}
		else if (Extention == "glb")
		{
			ObjectType = object::E3DObjectType::glb;
		}
		else if (Extention == "fbx")
		{
			ObjectType = object::E3DObjectType::Fbx;
		}
		else if (Extention == "pmx")
		{
			ObjectType = object::E3DObjectType::Pmx;
		}
		else
		{
			Console::Log("[Error - 3DObjectLoader] Unsupported file extention: %s\n", Extention.c_str());

			return false;
		}

		// バイナリデータを渡しておく. その解析は後で行う
		m_TargetObject->SetBinaryData(m_File->GetData(), m_FileName, ObjectType);

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;

		return true;
	}
}