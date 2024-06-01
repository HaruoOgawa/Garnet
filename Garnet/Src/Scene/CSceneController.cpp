#include "CSceneController.h"
#include "../Object/C3DObject.h"
#include "../Message/Console.h"

namespace scene
{
	CSceneController::CSceneController()
	{
	}

	CSceneController::~CSceneController()
	{
	}

	void CSceneController::AddObject(const std::shared_ptr<object::C3DObject>& Object)
	{
		m_ObjectList.push_back(Object);
	}

	std::vector<std::shared_ptr<object::C3DObject>> CSceneController::GetObjectList() const
	{
		return m_ObjectList;
	}

	void CSceneController::AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		m_MaterialFrameMap.emplace(MFName, MaterialFrame);
	}

	void CSceneController::AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SMaterialInfo>& MaterialInfoList)
	{
		m_MaterialInfoMap.emplace(Object, MaterialInfoList);
	}

	void CSceneController::AddTextureInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SLoadTextureInfo>& TextureInfoList)
	{
		m_TextureInfoMap.emplace(Object, TextureInfoList);
	}

	bool CSceneController::Create(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		for (const auto& Object : m_ObjectList)
		{
			// テクスチャの追加
			std::map<std::string, int> TexIndexMap;
			if (!PrepareTextureList(Object, TexIndexMap)) return false;
			
			// マテリアルの追加
			if (!PrepareMaterialList(pGraphicsAPI, Object, TexIndexMap)) return false;
			
			// Object生成
			if (!Object->Create(pGraphicsAPI, pPhysicsEngine, nullptr)) return false;
		}

		return true;
	}

	bool CSceneController::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}
	bool CSceneController::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->LateUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool CSceneController::PrepareTextureList(const std::shared_ptr<object::C3DObject>& Object, std::map<std::string, int>& TexIndexMap)
	{
		const auto& it = m_TextureInfoMap.find(Object);
		if (it != m_TextureInfoMap.end())
		{
			const auto& TextureInfoList = it->second;

			for (const auto& TextureInfo : TextureInfoList)
			{
				TexIndexMap.emplace(TextureInfo.TextureName, static_cast<int>(Object->GetTextureSet()->Get2DTextureList().size()));

				Object->GetTextureSet()->Add2DTexture(TextureInfo.Texture);
			}
		}

		return true;
	}

	bool CSceneController::PrepareMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap)
	{
		if (static_cast<int>(Object->GetMaterialList().size()) == 0)
		{
			// MaterialListを生成
			if (!CreateMaterialList(pGraphicsAPI, Object, TexIndexMap)) return false;
		}
		else
		{
			// Material情報を更新
			if (!UpdateMaterialUniform(pGraphicsAPI, Object, TexIndexMap)) return false;
		}

		return true;
	}

	bool CSceneController::CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap)
	{
		// MaterialInfoを取得
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end())
		{
			Console::Log("[SceneController Error] MaterialInfo Not Found\n");

			return false;
		}

		const auto& MaterialInfoList = it->second;

		for (const auto& MaterialInfo : MaterialInfoList)
		{
			// MaterialFrameを取得
			const auto& MaterialFrame = m_MaterialFrameMap.find(MaterialInfo.MaterialFrameName);
			if (MaterialFrame == m_MaterialFrameMap.end())
			{
				Console::Log("[SceneController Error] MaterialFrame Not Found\n");

				return false;
			}

			// Materialを生成
			auto Material = MaterialFrame->second->CreateMaterial(pGraphicsAPI, MaterialInfo.RefCount, MaterialInfo.CullMode);

			// UniformValueを設定
			for (const auto& UniformInfo : MaterialInfo.UniformInfoList)
			{
				Material->SetUniformValue(UniformInfo.UniformName, &UniformInfo.UniformData[0], UniformInfo.ByteSize);
			}

			// Textureを設定
			for (const auto& Texture : MaterialInfo.Textures)
			{
				const auto& TextureBufferName = std::get<0>(Texture);
				const auto& TextureName = std::get<1>(Texture);
				int Index = std::get<2>(Texture);

				int TextureIndex = -1;

				auto TexIndexIT = TexIndexMap.find(TextureName);
				if (TexIndexIT != TexIndexMap.end())
				{
					TextureIndex = TexIndexIT->second;
				}
				else if (Index != -1)
				{
					TextureIndex = Index;
				}

				if (TextureIndex == -1) continue;

				Material->ReplaceTextureIndex(TextureBufferName, TextureIndex);
			}

			// Materialを登録
			Object->AddMaterial(Material);
		}

		return true;
	}

	bool CSceneController::UpdateMaterialUniform(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap)
	{
		// MaterialInfoを取得
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end()) return true;

		//
		const auto& MaterialInfoList = it->second;
		const auto& MaterialList = Object->GetMaterialList();

		if (MaterialInfoList.size() != MaterialList.size()) return true;

		//
		for (int MaterialIndex = 0; MaterialIndex < static_cast<int>(MaterialList.size()); MaterialIndex++)
		{
			const auto& MaterialInfo = MaterialInfoList[MaterialIndex];
			const auto& Material = MaterialList[MaterialIndex];

			// UniformValueを設定
			for (const auto& UniformInfo : MaterialInfo.UniformInfoList)
			{
				Material->SetUniformValue(UniformInfo.UniformName, &UniformInfo.UniformData[0], UniformInfo.ByteSize);
			}
		}

		return true;
	}
}