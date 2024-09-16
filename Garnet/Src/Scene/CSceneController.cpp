#include "CSceneController.h"
#include "CSceneWriter.h"
#include <LoadWorker/CLoadWorker.h>
#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Audio/CAudioClip.h"
#include "../Message/Console.h"

namespace scene
{
	CSceneController::CSceneController():
		m_SceneTextureSet(nullptr),
		m_BGM(std::make_tuple(nullptr, false, false)),
		m_IsLoaded(false),
		m_TimelineFileName(std::string()),
		m_DefaultRenderPass(std::string()),
		m_DefaultDepthPass(std::string())
	{
	}

	CSceneController::~CSceneController()
	{
	}

	void CSceneController::SetDefaultPass(const std::string& RenderPass, const std::string& DepthPass)
	{
		m_DefaultRenderPass = RenderPass;
		m_DefaultDepthPass = DepthPass;
	}

	void CSceneController::SetFileName(const std::string& Name)
	{
		m_FileName = Name;
	}

	const std::string& CSceneController::GetFileName() const
	{
		return m_FileName;
	}

	void CSceneController::SetTimelineFileName(const std::string& Name)
	{
		m_TimelineFileName = Name;
	}

	const std::string& CSceneController::GetTimelineFileName() const
	{
		return m_TimelineFileName;
	}

	void CSceneController::AddObject(const std::shared_ptr<object::C3DObject>& Object)
	{
		// レンダーパス名が空ならデフォルトの値を設定する
		if (Object->GetPassName().empty())
		{
			Object->SetPassName(m_DefaultRenderPass);
		}

		if (Object->GetDepthPassName().empty())
		{
			Object->SetDepthPassName(m_DefaultDepthPass);
		}

		// SceneTextureSet
		if (m_SceneTextureSet)
		{
			for (const auto& CubeMap : m_SceneTextureSet->GetCubeMapList())
			{
				Object->GetTextureSet()->AddCubeMap(CubeMap);
			}

			const auto& Diffuse_Tex = m_SceneTextureSet->GetDiffuse_Tex();
			const auto& Specular_Tex = m_SceneTextureSet->GetSpecular_Tex();
			const auto& GGXLUT_Tex = m_SceneTextureSet->GetGGXLUT_Tex();
			if (Diffuse_Tex && Specular_Tex && GGXLUT_Tex)
			{
				Object->GetTextureSet()->AddIBLTexture(Diffuse_Tex, Specular_Tex, GGXLUT_Tex);
			}
		}

		m_ObjectList.push_back(Object);
	}

	void CSceneController::AddObjectWithLoading(resource::CLoadWorker* pLoadWorker, const std::shared_ptr<object::C3DObject>& Object, const std::string& FileName, const std::string& DefaultMaterialframeName)
	{
		const auto MaterialFrame = FindMaterialFrame(DefaultMaterialframeName);
		if (!MaterialFrame) return;

		// ObjectListに追加
		AddObject(Object);

		// ロードワーカーに渡してロード開始
		pLoadWorker->AddLoadResource(std::make_shared<resource::C3DObjectLoader>(FileName, Object, MaterialFrame, DefaultMaterialframeName));
	}

	std::vector<std::shared_ptr<object::C3DObject>> CSceneController::GetObjectList() const
	{
		return m_ObjectList;
	}

	std::shared_ptr<object::C3DObject> CSceneController::FindObjectByName(const std::string& Name)
	{
		std::shared_ptr<object::C3DObject> DstObject = nullptr;

		for (const auto& Object : m_ObjectList)
		{
			if (Object->GetObjectName() == Name)
			{
				DstObject = Object;
				break;
			}
		}

		return DstObject;
	}

	std::shared_ptr<object::C3DObject> CSceneController::FindObjectByIndex(int Index)
	{
		std::shared_ptr<object::C3DObject> DstObject = nullptr;

		if (Index >= 0 && Index < static_cast<int>(m_ObjectList.size()))
		{
			DstObject = m_ObjectList[Index];
		}

		return DstObject;
	}

	void CSceneController::RemoveObject(int Index)
	{
		if (Index < 0 || Index >= static_cast<int>(m_ObjectList.size())) return;

		m_ObjectList.erase(m_ObjectList.begin() + Index);
		m_ObjectList.shrink_to_fit();
	}

	void CSceneController::AddMaterialFrame(const std::string& MFName, const std::shared_ptr<graphics::CMaterialFrame>& MaterialFrame)
	{
		m_MaterialFrameMap.emplace(MFName, MaterialFrame);
	}

	void CSceneController::AddMaterialFrameWithLoading(resource::CLoadWorker* pLoadWorker, const std::string& MFName, const std::string& FileName)
	{
		const auto it = m_MaterialFrameMap.find(MFName);

		if (it == m_MaterialFrameMap.end())
		{
			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();

			pLoadWorker->AddLoadResource(std::make_shared<resource::CMaterialFrameLoader>(FileName, MaterialFrame));

			AddMaterialFrame(MFName, MaterialFrame);
		}
	}

	const std::map<std::string, std::shared_ptr<graphics::CMaterialFrame>>& CSceneController::GetMaterialFrameMap() const
	{
		return m_MaterialFrameMap;
	}

	std::shared_ptr<graphics::CMaterialFrame> CSceneController::FindMaterialFrame(const std::string& MFName)
	{
		std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = nullptr;

		const auto it = m_MaterialFrameMap.find(MFName);
		if (it != m_MaterialFrameMap.end()) MaterialFrame = it->second;

		return MaterialFrame;
	}

	void CSceneController::SetValueRegistry(const std::string& Registryname, const std::shared_ptr<scriptable::CValueRegistry>& ValueRegistry)
	{
		const auto it = m_ValueRegistryList.find(Registryname);
		if (it == m_ValueRegistryList.end())
		{
			// 新規追加する
			m_ValueRegistryList.emplace(Registryname, ValueRegistry);
		}
		else
		{
			// 以前の一部値を保持する
			std::vector<std::string> TrackIDList = m_ValueRegistryList[Registryname]->GetRefTrackIDList();

			// 値を更新する
			m_ValueRegistryList[Registryname] = ValueRegistry;
			m_ValueRegistryList[Registryname]->SetRefTrackIDList(TrackIDList);
		}
	}
	const std::map<std::string, std::shared_ptr<scriptable::CValueRegistry>>& CSceneController::GetValueRegistryList()
	{
		return m_ValueRegistryList;
	}

	void CSceneController::AddAnimationClipSet(const std::string& Name, const std::shared_ptr<animation::CAnimationClipSet>& AnimationClipSet)
	{
		m_AnimationClipSetMap.emplace(Name, AnimationClipSet);
	}

	const std::map<std::string, std::shared_ptr<animation::CAnimationClipSet>>& CSceneController::GetAnimationClipSetMap() const
	{
		return m_AnimationClipSetMap;
	}

	void CSceneController::SetSceneTextureSet(const std::shared_ptr<graphics::CTextureSet>& TextureSet)
	{
		m_SceneTextureSet = TextureSet;
	}

	const std::shared_ptr<graphics::CTextureSet>& CSceneController::GetSceneTextureSet() const
	{
		return m_SceneTextureSet;
	}

	void CSceneController::AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::vector<SMaterialInfo>& MaterialInfoList)
	{
		m_MaterialInfoMap.emplace(Object, MaterialInfoList);
	}

	const std::map<std::shared_ptr<object::C3DObject>, std::vector<SMaterialInfo>>& CSceneController::GetMaterialInfoMap() const
	{
		return m_MaterialInfoMap;
	}

	void CSceneController::AddTextureInfo(const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, std::shared_ptr<graphics::CTexture>>& TextureInfoList)
	{
		m_TextureInfoMap.emplace(Object, TextureInfoList);
	}

	const std::map<std::shared_ptr<object::C3DObject>, std::map<std::string, std::shared_ptr<graphics::CTexture>>>& CSceneController::GetTextureInfoMap() const
	{
		return m_TextureInfoMap;
	}

	void CSceneController::AddAnimationInfo(const std::shared_ptr<object::C3DObject>& Object, const SAnimationInfo& AnimationInfo)
	{
		m_AnimationInfoMap.emplace(Object, AnimationInfo);
	}

	const std::map<std::shared_ptr<object::C3DObject>, SAnimationInfo>& CSceneController::GetAnimationInfoMap() const
	{
		return m_AnimationInfoMap;
	}

	void CSceneController::AddBGM(const std::shared_ptr<audio::CAudioClip>& AudioClip, bool autoplay, bool loop)
	{
		m_BGM = std::make_tuple(AudioClip, autoplay, loop);
	}

	const std::tuple<std::shared_ptr<audio::CAudioClip>, bool, bool>& CSceneController::GetSound() const
	{
		return m_BGM;
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
			
#ifdef USE_ANIMATION
			// アニメーションを追加
			if (!PrepareAnimationList(pGraphicsAPI, Object)) return false;
#endif // USE_ANIMATION

			// Object生成
			if (!Object->Create(pGraphicsAPI, pPhysicsEngine, nullptr)) return false;
		}

		// Audio
		{
			const auto& AudioClip = std::get<0>(m_BGM);

			if (AudioClip)
			{
				bool autoplay = std::get<1>(m_BGM);
				bool loop = std::get<2>(m_BGM);

				if (autoplay)
				{
					if (loop)
					{
						AudioClip->PlayLoop();
					}
					else
					{
						AudioClip->PlayOneShot();
					}
				}
			}
		}

		m_IsLoaded = true;

		return true;
	}

	bool CSceneController::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_IsLoaded) return true;

#ifdef USE_BINARY_WRITE
		if (InputState->IsKeyDown(input::EKeyType::KEY_TYPE_CONTROL) && InputState->IsKeyUp(input::EKeyType::KEY_TYPE_S))
		{
			if (!CSceneWriter::Write(this)) return false;
		}
#endif // USE_BINARY_WRITE

		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::LateUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		for (const auto& Object : m_ObjectList)
		{
			if (!Object->LateUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::FixedUpdate(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		for (const auto& Object : m_ObjectList)
		{
			if (!Object->FixedUpdate(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime())) return false;
		}

		return true;
	}

	bool CSceneController::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Draw(pGraphicsAPI, IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
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
				TexIndexMap.emplace(TextureInfo.first, static_cast<int>(Object->GetTextureSet()->Get2DTextureList().size()));

				Object->GetTextureSet()->Add2DTexture(TextureInfo.second);
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

#ifdef USE_ANIMATION
	bool CSceneController::PrepareAnimationList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object)
	{
		const auto& it = m_AnimationInfoMap.find(Object);
		if (it == m_AnimationInfoMap.end()) return true;

		const auto& AnimationInfo = it->second;

		// humanoidclips
		for (const auto& Humanoidclip : AnimationInfo.Humanoidclips)
		{
			const auto& AnimationClipSet = m_AnimationClipSetMap.find(Humanoidclip.second.MotionName);
			if (AnimationClipSet == m_AnimationClipSetMap.end()) continue;

			const auto& Clip = AnimationClipSet->second->GetAnimationClip(Humanoidclip.second.Index);
			if (!Clip) continue;

			Object->AddHumanoidAnimationClip(Clip, Humanoidclip.second.Key, { nullptr, "" }, Humanoidclip.second.Loop, Humanoidclip.second.IK);
		}

		// blendshapes
		for (const auto& BlendshapeClip : AnimationInfo.Blendshapes)
		{
			const auto& AnimationClipSet = m_AnimationClipSetMap.find(BlendshapeClip.second.MotionName);
			if (AnimationClipSet == m_AnimationClipSetMap.end()) continue;

			const auto& Clip = AnimationClipSet->second->GetBlendShapeClip(BlendshapeClip.second.Index);
			if (!Clip) continue;

			Object->AddBlendShapeClip(Clip, BlendshapeClip.second.Key, BlendshapeClip.second.Loop);
		}

		if (!AnimationInfo.PlayMotion.empty())
		{
			Object->ChangeMotion(AnimationInfo.PlayMotion);
		}

		if (AnimationInfo.PlayMotionIndex != -1)
		{
			Object->ChangeMotion(AnimationInfo.PlayMotionIndex);
		}

		for (const std::string& playShape : AnimationInfo.PlayBlendShapes)
		{
			Object->PlayBlendShape(playShape);
		}

		return true;
	}
#endif // USE_ANIMATION

	bool CSceneController::CreateMaterialList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap)
	{
		// MaterialInfoを取得
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end())
		{
			Console::Log("[SceneController Error] MaterialInfo Not Found\n");

			return true;
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

			// CullMode
			Material->SetCullMode(MaterialInfo.CullMode);

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

			// TrackIDList
			Material->SetRefTrackIDList(MaterialInfo.TrackIDList);

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

			// TrackIDList
			Material->SetRefTrackIDList(MaterialInfo.TrackIDList);
		}

		return true;
	}
}