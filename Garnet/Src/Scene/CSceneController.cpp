#include "CSceneController.h"
#include <LoadWorker/CLoadWorker.h>
#include <LoadWorker/CMaterialFrameLoader.h>
#include "../Object/C3DObject.h"
#include "../Animation/CAnimationClipSet.h"
#include "../Audio/CAudioClip.h"
#include "../Message/Console.h"

namespace scene
{
	CSceneController::CSceneController():
		m_SceneTextureSet(std::make_shared<graphics::CTextureSet>()),
		m_BGM(std::make_tuple(nullptr, false, false)),
		m_IsLoaded(false),
		m_TimelineFileName(std::string()),
		m_DefaultRenderPass(std::string())
	{

	}

	CSceneController::~CSceneController()
	{
	}

	void CSceneController::Reset()
	{
		for (auto& Object : m_ObjectList)
		{
			Object->Reset();
		}
	}

	void CSceneController::SetDefaultPass(const std::string& RenderPass)
	{
		m_DefaultRenderPass = RenderPass;
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
		if (Object->GetPassNameList().empty())
		{
			Object->AddPassName(m_DefaultRenderPass);
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

			for (const auto& FrameTexture : m_SceneTextureSet->GetFrameTextureList())
			{
				Object->GetTextureSet()->AddFrameTexture(FrameTexture);
			}
		}

		m_ObjectList.push_back(Object);
	}

	void CSceneController::AddObjectWithLoading(resource::CLoadWorker* pLoadWorker, const std::shared_ptr<object::C3DObject>& Object, const std::string& FileName,
		const std::vector<std::string>& defaultmaterialframeList, animation::ERigType RigType)
	{
		std::vector<std::shared_ptr<graphics::CMaterialFrame>> BaseMaterialFrameList;
		
		for (const auto& Name : defaultmaterialframeList)
		{
			const auto MaterialFrame = FindMaterialFrame(Name);
			if (!MaterialFrame) continue;

			BaseMaterialFrameList.push_back(MaterialFrame);
		}

		if (BaseMaterialFrameList.empty()) return;

		// ObjectListに追加
		AddObject(Object);

		// ロードワーカーに渡してロード開始
		pLoadWorker->AddLoadResource(std::make_shared<resource::C3DObjectLoader>(FileName, Object, BaseMaterialFrameList, defaultmaterialframeList, 
			RigType, std::map<animation::EHumanoidBones, std::string>(), 1));
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

	void CSceneController::AddMaterialFrameWithLoading(resource::CLoadWorker* pLoadWorker, const std::string& FileName)
	{
		const auto it = m_MaterialFrameLoaderMap.find(FileName);

		if (it == m_MaterialFrameLoaderMap.end())
		{
			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
			std::shared_ptr<resource::CMaterialFrameLoader> MaterialFrameLoader = std::make_shared<resource::CMaterialFrameLoader>(FileName, MaterialFrame);

			pLoadWorker->AddLoadResource(MaterialFrameLoader);

			m_MaterialFrameLoaderMap.emplace(FileName, MaterialFrameLoader);
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

	const std::shared_ptr<graphics::CTextureSet>& CSceneController::GetSceneTextureSet() const
	{
		return m_SceneTextureSet;
	}

	std::shared_ptr<graphics::CTextureSet>& CSceneController::GetSceneTextureSet()
	{
		return m_SceneTextureSet;
	}

	void CSceneController::AddFrameTexture(const std::shared_ptr<graphics::CTexture>& Texture)
	{
		m_SceneTextureSet->AddFrameTexture(Texture);
	}

	void CSceneController::AddMaterialInfo(const std::shared_ptr<object::C3DObject>& Object, const std::map<std::tuple<int, int>, std::vector<SMaterialInfo>>& MaterialInfoList)
	{
		m_MaterialInfoMap.emplace(Object, MaterialInfoList);
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


			// コンポーネントを追加 Component
			for (size_t NodeIndex = 0; NodeIndex < Object->GetNodeList().size(); NodeIndex++)
			{
				for (const auto& Component : Object->GetNodeList()[NodeIndex]->GetComponentList())
				{
					// ValueRegistryの登録
					{
						const auto& RegistryName = Component->GetRegistryName();
						if (RegistryName.empty()) continue;

						const auto& ValueRegistry = m_ValueRegistryList.find(RegistryName);
						if (ValueRegistry == m_ValueRegistryList.end()) continue;

						Component->SetValueRegistry(ValueRegistry->second);
					}

					// OnLoadedを実行
					if (!Component->OnLoaded(pGraphicsAPI, shared_from_this(), Object, Object->GetNodeList()[NodeIndex]))
					{
						Console::Log("[Error] Faield to load Component.\n");
						return false;
					}
				}
			}

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

#ifdef _DEBUG
		if(!m_MaterialFrameMap.empty())
		{
			// Materialを生成
			// 適当に最初のマテリアルを使う
			auto MaterialFrame = m_MaterialFrameMap.find("pbr_mat");
			if (MaterialFrame != m_MaterialFrameMap.end())
			{
				auto Material = MaterialFrame->second->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_BACK);

				Material->SetDepthFunc(graphics::EDepthFunc::Always);

				m_DebugSphere = std::make_shared<object::C3DObject>();
				m_DebugSphere->AddPassName(m_DefaultRenderPass);
				if (!m_DebugSphere->CreatePresetSimply(pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), graphics::EPresetPrimitiveType::SPHERE, Material, nullptr)) return false;
			}
		}
#endif // _DEBUG

		m_IsLoaded = true;

		return true;
	}

	bool CSceneController::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState, const std::shared_ptr<timeline::CTimelineController>& TimelineController)
	{
		// Loading Check
		{
			for (auto it = m_MaterialFrameLoaderMap.begin(); it != m_MaterialFrameLoaderMap.end(); )
			{
				if (it->second->IsLoaded())
				{
					for (const auto& MaterialFrame : it->second->GetTargetMaterialFrameSet())
					{
						// 既に存在するならスキップ
						if (m_MaterialFrameMap.find(MaterialFrame->GetMaterialFrameName()) != m_MaterialFrameMap.end()) continue;

						AddMaterialFrame(MaterialFrame->GetMaterialFrameName(), MaterialFrame);
					}

					m_MaterialFrameLoaderMap.erase(it++);
				}
				else
				{
					it++;
				}
			}
		}

		//
		if (!m_IsLoaded) return true;

		//
		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime(), pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		}

#ifdef _DEBUG
		if (m_DebugSphere)
		{
			if (!m_DebugSphere->Update(pGraphicsAPI, pPhysicsEngine, DrawInfo->GetDeltaSecondsTime(), pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		}
#endif

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

	bool CSceneController::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		for (const auto& Object : m_ObjectList)
		{
			if (!Object->Draw(pGraphicsAPI, Camera, Projection, DrawInfo, m_DebugSphere)) return false;
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
		if (!Object->GetFileName().empty())
		{
			// ファイルからダウンロードしたモデルであればMaterial情報の更新のみを行う
			if (!UpdateMaterialUniform(pGraphicsAPI, Object, TexIndexMap)) return false;
		}
		else
		{
			// MaterialListを生成
			if (!CreateMaterialList(pGraphicsAPI, Object, TexIndexMap)) return false;
		}

		return true;
	}

#ifdef USE_ANIMATION
	bool CSceneController::PrepareAnimationList(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object)
	{
		const auto& it = m_AnimationInfoMap.find(Object);
		if (it == m_AnimationInfoMap.end()) return true;

		const auto& AnimationInfo = it->second;

		// clips(通常のスキンメッシュアニメーション)
		for (const auto& clip : AnimationInfo.Clips)
		{
			const auto& AnimationClipSet = m_AnimationClipSetMap.find(clip.second.MotionName);
			if (AnimationClipSet == m_AnimationClipSetMap.end()) continue;

			const auto& Clip = AnimationClipSet->second->GetAnimationClip(clip.second.Index);
			if (!Clip) continue;

			Object->AddAnimationClip(Clip, clip.second.Key, { nullptr, "" }, clip.second.Loop);
		}

		// humanoidclips(ヒューマノイドアニメーション)
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

		const auto& RendererKeyMaterialInfoList = it->second;

		for (const auto& MaterialInfoList : RendererKeyMaterialInfoList)
		{
			for (const auto& MaterialInfo : MaterialInfoList.second)
			{
				// MaterialFrameを取得
				const auto& MaterialFrame = m_MaterialFrameMap.find(MaterialInfo.MaterialFrameName);
				if (MaterialFrame == m_MaterialFrameMap.end())
				{
					Console::Log("[SceneController Error] MaterialFrame Not Found\n");

					return false;
				}

				// Materialを生成
				auto Material = MaterialFrame->second->CreateMaterial(pGraphicsAPI, MaterialInfo.CullMode);

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
				if (MaterialInfo.MeshIndex < 0 || MaterialInfo.MeshIndex >= static_cast<int>(Object->GetMeshList().size())) return false;
				const auto& Mesh = Object->GetMeshList()[MaterialInfo.MeshIndex];

				if (MaterialInfo.PrimitiveIndex < 0 || MaterialInfo.PrimitiveIndex >= static_cast<int>(Mesh->GetPrimitiveList().size())) return false;
				const auto& Primitive = Mesh->GetPrimitiveList()[MaterialInfo.PrimitiveIndex];

				Primitive->AddMaterial(pGraphicsAPI, Material);
			}
		}

		return true;
	}

	bool CSceneController::UpdateMaterialUniform(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<object::C3DObject>& Object, const std::map<std::string, int>& TexIndexMap)
	{
		// MaterialInfoを取得
		const auto& it = m_MaterialInfoMap.find(Object);
		if (it == m_MaterialInfoMap.end()) return true;

		const auto& RendererKeyMaterialInfoList = it->second;

		const auto& MeshList = Object->GetMeshList();
		for (int MeshIndex = 0; MeshIndex < static_cast<int>(MeshList.size()); MeshIndex++)
		{
			const auto& Mesh = Object->GetMeshList()[MeshIndex];

			const auto& PrimitiveList = Mesh->GetPrimitiveList();
			for (int PrimitiveIndex = 0; PrimitiveIndex < static_cast<int>(PrimitiveList.size()); PrimitiveIndex++)
			{
				const auto& Primitive = Mesh->GetPrimitiveList()[PrimitiveIndex];

				std::tuple<int, int> RendererKey = std::make_tuple(MeshIndex, PrimitiveIndex);
				const auto& MaterialInfoList = RendererKeyMaterialInfoList.find(RendererKey);
				if (MaterialInfoList == RendererKeyMaterialInfoList.end()) continue;

				const auto& RendererList = Primitive->GetRendererList();
				for (int RendererIndex = 0; RendererIndex < static_cast<int>(RendererList.size()); RendererIndex++)
				{
					const auto& Renderer = RendererList[RendererIndex];

					if (RendererIndex >= static_cast<int>(MaterialInfoList->second.size())) continue;
					const auto& MaterialInfo = MaterialInfoList->second[RendererIndex];

					auto Material = std::get<1>(Renderer);
					if (!Material) continue;

					// マテリアルを差し替える
					bool MatReplaced = false;
					if (MaterialInfo.MaterialFrameName != Material->GetMaterialFrame()->GetMaterialFrameName())
					{
						const auto& OldMF = Material->GetMaterialFrame();
						const auto& NewMF = m_MaterialFrameMap.find(MaterialInfo.MaterialFrameName);
						if (NewMF == m_MaterialFrameMap.end()) continue;

						OldMF->DeleteRefMaterial(Material);

						auto NewMat = NewMF->second->CreateMaterial(pGraphicsAPI, NewMF->second->GetCullMode());
						
						Primitive->ReplaceMaterial(Renderer, NewMat);

						Material = NewMat;

						MatReplaced = true;
					}

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

					// マテリアルを置き換えたので再生成する
					if (MatReplaced)
					{
						if (!Material->Create(Object->GetPassNameList(), Object->GetTextureSet())) return false;
					}
				}
			}
		}

		return true;
	}
}