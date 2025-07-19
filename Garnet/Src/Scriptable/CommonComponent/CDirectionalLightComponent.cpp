#include "CDirectionalLightComponent.h"

#include <LoadWorker/CLoadWorker.h>
#include <Object/C3DObject.h>
#include <Graphics/CMesh.h>

namespace scriptable
{
	CDirectionalLightComponent::CDirectionalLightComponent(const std::string& ComponentName, const std::string& RegistryName):
		CComponent(ComponentName, RegistryName),
		m_Status(resource::ELoadStatus::None),
		m_Loader(nullptr),
		m_LightObject(nullptr),
		m_Material(nullptr)
	{
		GetValueRegistry()->SetValue("intensity", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(1.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("dir", graphics::EUniformValueType::VALUE_TYPE_VEC4, &glm::vec4(2.358f, -15.6f, 0.59f, 0.0f)[0], sizeof(float) * 4);
		GetValueRegistry()->SetValue("color", graphics::EUniformValueType::VALUE_TYPE_VEC4, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(float) * 4);
	}

	CDirectionalLightComponent::~CDirectionalLightComponent()
	{
	}

	bool CDirectionalLightComponent::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}

	bool CDirectionalLightComponent::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		std::string filename = "Resources/MaterialFrame/DirectionalLight_MF.json";

		std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
		m_Loader = std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame);

		pLoadWorker->AddLoadResource(m_Loader);

		m_Status = resource::ELoadStatus::Loading;

		return true;
	}

	bool CDirectionalLightComponent::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, 
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
		else if(Object)
		{
			m_LightObject->SetPos(Object->GetPos());
			m_LightObject->SetRot(Object->GetRot());
			m_LightObject->SetScale(Object->GetScale());
		}

		if (!m_LightObject->Update(pGraphicsAPI, pPhysicsEngine, 0.0f, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		if (m_Material)
		{
			float intensity = GetValueRegistry()->GetValueFloat("intensity");
			std::vector<float> dir = GetValueRegistry()->GetValueVec4("dir");
			std::vector<float> color = GetValueRegistry()->GetValueVec4("color");

			m_Material->SetUniformValue("intensity", &glm::vec1(intensity)[0], sizeof(float));
			m_Material->SetUniformValue("dir", &dir[0], sizeof(float) * dir.size());
			m_Material->SetUniformValue("color", &color[0], sizeof(float) * color.size());
		}

		return true;
	}

	bool CDirectionalLightComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (m_Status != resource::ELoadStatus::Loaded) return true;
		if (!m_LightObject) return true;

		if (!m_LightObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}

	bool CDirectionalLightComponent::CheckIsLoading(bool& Loaded, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
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
		m_LightObject->AddPassName("GBufferLightPass");

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
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_NONE);

			// 他のライトが描画できなくなるので書き込まない
			Material->SetEnabledZWrite(false);

			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_ADDITIVE);
			Material->SetEnabledZTest(false);
			Material->ReplaceTextureIndex("gPositionTexture", 0);
			Material->ReplaceTextureIndex("gNormalTexture", 1);
			Material->ReplaceTextureIndex("gAlbedoTexture", 2);
			Material->ReplaceTextureIndex("gDepthTexture", 3);
			Material->ReplaceTextureIndex("gCustomParam0Texture", 4);

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightObject->CreatePresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD, Material)) return false;
		
			m_Material = Material;

			// 1つ分しか見ない
			break;
		}

		return true;
	}
}