#include "CSpotLightComponent.h"

#include <LoadWorker/CLoadWorker.h>
#include <Object/C3DObject.h>
#include <Graphics/CMesh.h>

namespace scriptable
{
	CSpotLightComponent::CSpotLightComponent(const std::string& ComponentName, const std::string& RegistryName) :
		CComponent(ComponentName, RegistryName),
		m_Status(resource::ELoadStatus::None),
		m_Loader(nullptr),
		m_SecondLoader(nullptr),
		m_LightObject(nullptr)
	{
		GetValueRegistry()->SetValue("intensity", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(1.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("color", graphics::EUniformValueType::VALUE_TYPE_VEC4, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(float) * 4);
		GetValueRegistry()->SetValue("angle", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(45.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("height", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(1.0f)[0], sizeof(float));
	}

	CSpotLightComponent::~CSpotLightComponent()
	{
	}

	bool CSpotLightComponent::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}

	bool CSpotLightComponent::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		// GBuffer Draw
		{
			std::string filename = "Resources/MaterialFrame/SpotLight_MF.json";

			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
			m_Loader = std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame);

			pLoadWorker->AddLoadResource(m_Loader);
		}

		// SpotLight Geometry
		{
			std::string filename = "Resources/MaterialFrame/SpotLight_Geom_MF.json";

			std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
			m_SecondLoader = std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame);

			pLoadWorker->AddLoadResource(m_SecondLoader);
		}

		m_Status = resource::ELoadStatus::Loading;

		return true;
	}

	bool CSpotLightComponent::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		bool Loaded = false;
		if (!CheckIsLoading(Loaded, pGraphicsAPI, pPhysicsEngine, pLoadWorker, Camera, Projection, DrawInfo, InputState, Object)) return false;

		if (!Loaded) return true;

		if (!m_LightObject) return true;

		if (SelfNode)
		{
			m_LightObject->SetPos(SelfNode->GetPos());
			m_LightObject->SetRot(SelfNode->GetRot());
			m_LightObject->SetScale(SelfNode->GetScale());
		}
		else if (Object)
		{
			m_LightObject->SetPos(Object->GetPos());
			m_LightObject->SetRot(Object->GetRot());
			m_LightObject->SetScale(Object->GetScale());
		}

		if (!m_LightObject->Update(pGraphicsAPI, pPhysicsEngine, 0.0f, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		for (auto& Material : m_MaterialList)
		{
			if (Material)
			{
				// angle
				float angle = GetValueRegistry()->GetValueFloat("angle");
				Material->SetUniformValue("angle", &glm::vec1(angle)[0], sizeof(float));

				// height
				float height = GetValueRegistry()->GetValueFloat("height");
				Material->SetUniformValue("height", &glm::vec1(height)[0], sizeof(float));

				// pos
				glm::vec3 Pos = m_LightObject->GetPos();
				Material->SetUniformValue("pos", &glm::vec4(Pos.x, Pos.y, Pos.z, 1.0f)[0], sizeof(float) * 4);

				// intensity
				float intensity = GetValueRegistry()->GetValueFloat("intensity");
				Material->SetUniformValue("intensity", &glm::vec1(intensity)[0], sizeof(float));

				// color
				std::vector<float> color = GetValueRegistry()->GetValueVec4("color");
				Material->SetUniformValue("color", &color[0], sizeof(float) * static_cast<int>(color.size()));

				// dir
				glm::vec4 dir = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
				const auto& WorldMatrix = m_LightObject->GetObjectTransform()->GetModelMatrix();
				dir = WorldMatrix * dir;
				dir = glm::normalize(dir);
				Material->SetUniformValue("dir", &dir[0], sizeof(float) * 4);
			}
		}
		
		return true;
	}

	bool CSpotLightComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (m_Status != resource::ELoadStatus::Loaded) return true;
		if (!m_LightObject) return true;

		if (!m_LightObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}

	bool CSpotLightComponent::CheckIsLoading(bool& Loaded, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState, const std::shared_ptr<object::C3DObject>& Object)
	{
		// ロード済み
		if (m_Status == resource::ELoadStatus::Loaded)
		{
			Loaded = true;
			return true;
		}

		// ロード中
		if (!m_Loader) return true;
		if (!m_Loader->IsLoaded()) return true;
		
		if (!m_SecondLoader) return true;
		if (!m_SecondLoader->IsLoaded()) return true;

		// ロード完了
		m_Status = resource::ELoadStatus::Loaded;
		Loaded = true;

		m_LightObject = std::make_shared<object::C3DObject>();

		// PassName
		for (const auto& PassName : Object->GetPassNameList())
		{
			m_LightObject->AddPassName(PassName);
		}

		// TextureList
		const auto& RenderPass = pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass");
		if (!RenderPass) return false;

		const auto& TextureList = RenderPass->GetFrameTextureList();
		if (TextureList.size() != 5) return false;

		for (const auto& Texture : TextureList)
		{
			m_LightObject->GetTextureSet()->AddFrameTexture(Texture);
		}

		// Mesh & Material
		for (const auto& MaterialFrame : m_Loader->GetTargetMaterialFrameSet())
		{
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_BACK);
			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_ADDITIVE);

			// 他のライトが描画できなくなるのでZTestはしない
			Material->SetEnabledZTest(false);

			Material->ReplaceTextureIndex("gPositionTexture", 0);
			Material->ReplaceTextureIndex("gNormalTexture", 1);
			Material->ReplaceTextureIndex("gAlbedoTexture", 2);
			Material->ReplaceTextureIndex("gDepthTexture", 3);
			Material->ReplaceTextureIndex("gCustomParam0Texture", 4);

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightObject->AddPresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateCylinder(pGraphicsAPI), graphics::EPresetPrimitiveType::CYLINDER, Material)) return false;

			m_MaterialList.push_back(Material);

			// 1つ分しか見ない
			break;
		}

		for (const auto& MaterialFrame : m_SecondLoader->GetTargetMaterialFrameSet())
		{
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_BACK);
			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA);

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightObject->AddPresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateCylinder(pGraphicsAPI), graphics::EPresetPrimitiveType::CYLINDER, Material)) return false;

			m_MaterialList.push_back(Material);

			// 1つ分しか見ない
			break;
		}

		// 生成
		if (!m_LightObject->Create(pGraphicsAPI, pPhysicsEngine)) return false;

		return true;
	}
}