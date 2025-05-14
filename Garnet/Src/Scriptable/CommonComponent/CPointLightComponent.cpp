#include "CPointLightComponent.h"

#include <LoadWorker/CLoadWorker.h>
#include <Object/C3DObject.h>
#include <Graphics/CMesh.h>

namespace scriptable
{
	CPointLightComponent::CPointLightComponent(const std::string& ComponentName, const std::string& RegistryName) :
		CComponent(ComponentName, RegistryName),
		m_Status(resource::ELoadStatus::None),
		m_Loader(nullptr),
		m_LightObject(nullptr),
		m_Material(nullptr)
	{
	}

	CPointLightComponent::~CPointLightComponent()
	{
	}

	bool CPointLightComponent::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}

	bool CPointLightComponent::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		std::string filename = "Resources/MaterialFrame/PointLight_MF.json";

		std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
		m_Loader = std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame);

		pLoadWorker->AddLoadResource(m_Loader);

		m_Status = resource::ELoadStatus::Loading;

		return true;
	}

	bool CPointLightComponent::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
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

		// カメラが球の中に突入したら面を裏返す
		float radius = 1.0f * m_LightObject->GetScale().x + Projection->GetNear();
		float dist = glm::distance(Camera->GetPos(), m_LightObject->GetPos());
		const bool IsFlip = (dist <= radius);

		if (IsFlip)
		{
			m_LightObject->SetScale(-1.0f * m_LightObject->GetScale());
		}

		if (!m_LightObject->Update(pGraphicsAPI, pPhysicsEngine, 0.0f, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		if (m_Material)
		{
			glm::vec3 Pos = m_LightObject->GetPos();
			glm::vec3 Scale = m_LightObject->GetScale();
			float MaxScale = fmaxf(Scale.x, fmaxf(Scale.y, Scale.z));

			m_Material->SetUniformValue("radius", &glm::vec1(MaxScale)[0], sizeof(float));
			m_Material->SetUniformValue("pos", &glm::vec4(Pos.x, Pos.y, Pos.z, 1.0f)[0], sizeof(float) * 4);
		}

		return true;
	}

	bool CPointLightComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (m_Status != resource::ELoadStatus::Loaded) return true;
		if (!m_LightObject) return true;

		if (!m_LightObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}

	bool CPointLightComponent::CheckIsLoading(bool& Loaded, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
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

			Material->ReplaceTextureIndex("gPositionTexture", 0);
			Material->ReplaceTextureIndex("gNormalTexture", 1);
			Material->ReplaceTextureIndex("gAlbedoTexture", 2);
			Material->ReplaceTextureIndex("gDepthTexture", 3);
			Material->ReplaceTextureIndex("gCustomParam0Texture", 4);

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightObject->CreatePresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateSphere(pGraphicsAPI), graphics::EPresetPrimitiveType::SPHERE, Material)) return false;

			m_Material = Material;

			// 1つ分しか見ない
			break;
		}

		return true;
	}
}