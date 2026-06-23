#include "CIndirectLightComponent.h"

#include <LoadWorker/CLoadWorker.h>
#include <Object/C3DObject.h>
#include <Graphics/CMesh.h>

namespace scriptable
{
	CIndirectLightComponent::CIndirectLightComponent(const std::string& ComponentName, const std::string& RegistryName) :
		CComponent(ComponentName, RegistryName),
		m_Status(resource::ELoadStatus::None),
		m_Loader(nullptr),
		m_LightObject(nullptr),
		m_Material(nullptr)
	{
		std::string DefferdPassName = "GBufferGenPass";
		std::string IndirectLightPassName = "GBufferIndirectLightPass";
		std::string LightingPassName = "GBufferLightPass";

		GetValueRegistry()->SetValue("DefferdPassName", graphics::EUniformValueType::VALUE_TYPE_STRING, DefferdPassName.c_str(), sizeof(char) * DefferdPassName.size());
		GetValueRegistry()->SetValue("LightingPassName", graphics::EUniformValueType::VALUE_TYPE_STRING, LightingPassName.c_str(), sizeof(char) * LightingPassName.size());
		GetValueRegistry()->SetValue("IndirectLightPassName", graphics::EUniformValueType::VALUE_TYPE_STRING, IndirectLightPassName.c_str(), sizeof(char) * IndirectLightPassName.size());
	}

	CIndirectLightComponent::~CIndirectLightComponent()
	{
	}

	bool CIndirectLightComponent::OnLoaded(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<scene::CSceneController>& SceneController,
		const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		return true;
	}

	bool CIndirectLightComponent::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		std::string filename = "Resources\\Common\\MaterialFrame\\IndirectLight_MF.json";

		std::shared_ptr<graphics::CMaterialFrame> MaterialFrame = std::make_shared<graphics::CMaterialFrame>();
		m_Loader = std::make_shared<resource::CMaterialFrameLoader>(filename, MaterialFrame);

		pLoadWorker->AddLoadResource(m_Loader);

		m_Status = resource::ELoadStatus::Loading;

		return true;
	}

	bool CIndirectLightComponent::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
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

		return true;
	}

	bool CIndirectLightComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (m_Status != resource::ELoadStatus::Loaded) return true;
		if (!m_LightObject) return true;

		if (!Object->IsEnabled() || !SelfNode->IsEnabled()) return true;

		if (!m_LightObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}

	bool CIndirectLightComponent::CheckIsLoading(bool& Loaded, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
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

		const auto& SrcObjectTextureSet = Object->GetTextureSet();

		// パス名を取得
		std::string DefferdPassName = GetValueRegistry()->GetValueString("DefferdPassName");
		std::string LightingPassName = GetValueRegistry()->GetValueString("LightingPassName");
		std::string IndirectLightPassName = GetValueRegistry()->GetValueString("IndirectLightPassName");

		// PassName
		m_LightObject->AddPassName(IndirectLightPassName);

		// FrameTextureList
		{
			// GBufferTexture
			const auto& RenderPass = pGraphicsAPI->FindOffScreenRenderPass(DefferdPassName);
			if (!RenderPass) return false;

			const auto& TextureList = RenderPass->GetFrameTextureList();
			if (TextureList.size() != 6) return false;

			for (const auto& Texture : TextureList)
			{
				m_LightObject->GetTextureSet()->AddFrameTexture(DefferdPassName, Texture);
			}
		}

		{
			// DirectLightTexture
			const auto& RenderPass = pGraphicsAPI->FindOffScreenRenderPass(LightingPassName);
			if (!RenderPass) return false;

			const auto& Texture = RenderPass->GetFrameTexture();
			if (!Texture) return false;

			m_LightObject->GetTextureSet()->AddFrameTexture(LightingPassName, Texture);
		}

		// IBL設定
		std::shared_ptr<graphics::CTexture> Diffuse_Tex = nullptr;
		if (SrcObjectTextureSet) Diffuse_Tex = SrcObjectTextureSet->GetDiffuse_Tex();

		std::shared_ptr<graphics::CTexture> Specular_Tex = nullptr;
		if (SrcObjectTextureSet) Specular_Tex = SrcObjectTextureSet->GetSpecular_Tex();

		std::shared_ptr<graphics::CTexture> GGXLUT_Tex = nullptr;
		if (SrcObjectTextureSet) GGXLUT_Tex = SrcObjectTextureSet->GetGGXLUT_Tex();

		const bool ExistIBL = (Diffuse_Tex && Specular_Tex && GGXLUT_Tex);
		if (ExistIBL)
		{
			m_LightObject->GetTextureSet()->AddIBLTexture(Diffuse_Tex, Specular_Tex, GGXLUT_Tex);
		}

		const int LightUBOBindingIndex = 1;

		// Mesh & Material
		for (const auto& MaterialFrame : m_Loader->GetTargetMaterialFrameSet())
		{
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_NONE);

			// GBuffer生成パスで既に深度が決まっているのでここで深度テストは行わない
			// また、ライトの描画範囲決定に影響がでることも理由の1つ
			Material->SetEnabledZTest(false);

			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_ADDITIVE);
			Material->ReplaceTextureIndex("gPositionTexture", 0);
			Material->ReplaceTextureIndex("gNormalTexture", 1);
			Material->ReplaceTextureIndex("gAlbedoTexture", 2);
			Material->ReplaceTextureIndex("gDepthTexture", 3);
			Material->ReplaceTextureIndex("gCustomParam0Texture", 4);
			Material->ReplaceTextureIndex("gEmissionTexture", 5);

			// IBL
			if (ExistIBL)
			{
				Material->ReplaceTextureIndex("IBL_Diffuse_Texture", 0);
				Material->ReplaceTextureIndex("IBL_Specular_Texture", 0);
				Material->ReplaceTextureIndex("IBL_GGXLUT_Texture", 0);

				Material->ReplacePreloadUniformValue("useIBL", &glm::ivec1(1)[0], sizeof(int), LightUBOBindingIndex);

				// MipCountには反射キューブマップかIBLのSpecularMapの値が入っている(これらは必ずどちらか一方しか使用されないため)
				float MipCount = 1.0f;
				if (Specular_Tex)
				{
					MipCount = Specular_Tex->GetMipCount();
				}
				Material->ReplacePreloadUniformValue("mipCount", &glm::vec1(MipCount)[0], sizeof(float), LightUBOBindingIndex);
			}

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightObject->CreatePresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD, Material)) return false;

			m_Material = Material;

			// 1つ分しか見ない
			break;
		}

		return true;
	}
}