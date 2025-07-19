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
		m_LightObject(nullptr),
		m_LightGeomObject(nullptr)
	{
		GetValueRegistry()->SetValue("intensity", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(1.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("color", graphics::EUniformValueType::VALUE_TYPE_VEC4, &glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)[0], sizeof(float) * 4);
		GetValueRegistry()->SetValue("angle", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(45.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("height", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(1.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("pan", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(0.0f)[0], sizeof(float));
		GetValueRegistry()->SetValue("tilt", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(0.0f)[0], sizeof(float));
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

		if (!m_LightObject || !m_LightGeomObject) return true;

		if (SelfNode)
		{
			m_LightObject->SetPos(SelfNode->GetPos());
			m_LightObject->SetRot(SelfNode->GetRot());
			m_LightObject->SetScale(SelfNode->GetScale());
			
			m_LightGeomObject->SetPos(SelfNode->GetPos());
			m_LightGeomObject->SetRot(SelfNode->GetRot());
			m_LightGeomObject->SetScale(SelfNode->GetScale());
		}
		else if (Object)
		{
			m_LightObject->SetPos(Object->GetPos());
			m_LightObject->SetRot(Object->GetRot());
			m_LightObject->SetScale(Object->GetScale());
			
			m_LightGeomObject->SetPos(Object->GetPos());
			m_LightGeomObject->SetRot(Object->GetRot());
			m_LightGeomObject->SetScale(Object->GetScale());
		}

		//
		if (!m_LightObject->Update(pGraphicsAPI, pPhysicsEngine, 0.0f, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;
		if (!m_LightGeomObject->Update(pGraphicsAPI, pPhysicsEngine, 0.0f, pLoadWorker, Camera, Projection, DrawInfo, InputState)) return false;

		// angle
		float angle = GetValueRegistry()->GetValueFloat("angle");

		// height
		float height = GetValueRegistry()->GetValueFloat("height");

		// pan
		float pan = GetValueRegistry()->GetValueFloat("pan");

		// tilt
		float tilt = GetValueRegistry()->GetValueFloat("tilt");

		// pos
		glm::vec3 Pos = m_LightObject->GetPos();

		// intensity
		float intensity = GetValueRegistry()->GetValueFloat("intensity");

		// color
		std::vector<float> color = GetValueRegistry()->GetValueVec4("color");

		// dir
		glm::vec4 dir = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
		const auto& WorldMatrix = m_LightObject->GetObjectTransform()->GetModelMatrix();
		glm::mat4 RotPan = glm::toMat4(glm::angleAxis(-pan, glm::vec3(1.0f, 0.0f, 0.0f)));
		glm::mat4 RotTilt = glm::toMat4(glm::angleAxis(tilt, glm::vec3(0.0f, 1.0f, 0.0f)));

		dir = WorldMatrix * RotTilt * RotPan * dir;
		dir = glm::normalize(dir);

		//
		for (auto& Material : m_MaterialList)
		{
			if (Material)
			{
				Material->SetUniformValue("angle", &glm::vec1(angle)[0], sizeof(float));
				Material->SetUniformValue("height", &glm::vec1(height)[0], sizeof(float));
				Material->SetUniformValue("pan", &glm::vec1(pan)[0], sizeof(float));
				Material->SetUniformValue("tilt", &glm::vec1(tilt)[0], sizeof(float));
				Material->SetUniformValue("pos", &glm::vec4(Pos.x, Pos.y, Pos.z, 1.0f)[0], sizeof(float) * 4);
				Material->SetUniformValue("intensity", &glm::vec1(intensity)[0], sizeof(float));
				Material->SetUniformValue("color", &color[0], sizeof(float) * static_cast<int>(color.size()));
				Material->SetUniformValue("dir", &dir[0], sizeof(float) * 4);
			}
		}
		
		return true;
	}

	bool CSpotLightComponent::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<object::C3DObject>& Object, const std::shared_ptr<object::CNode>& SelfNode)
	{
		if (m_Status != resource::ELoadStatus::Loaded) return true;
		if (!m_LightObject || !m_LightGeomObject) return true;

		if (!m_LightObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;
		if (!m_LightGeomObject->Draw(pGraphicsAPI, Camera, Projection, DrawInfo)) return false;

		return true;
	}

#ifdef USE_NETWORK
	void CSpotLightComponent::OnReceiveDMXData(const network::SDMXFixture& Fixture, const std::vector<unsigned char>& DMXData)
	{
		if (Fixture.DeviceName == "DefaultSpotLight")
		{
			if (Fixture.ChannelNameList.size() != 8) return;

			// Color
			float R = static_cast<float>(DMXData[0]) / 255.0f;
			float G = static_cast<float>(DMXData[1]) / 255.0f;
			float B = static_cast<float>(DMXData[2]) / 255.0f;

			std::vector<float> color = { R, G, B, 1.0f };

			GetValueRegistry()->SetValue("color", graphics::EUniformValueType::VALUE_TYPE_VEC4, &color[0], sizeof(float) * static_cast<int>(color.size()));

			// Dimmer(intensity)
			// 10.0まで明るさが指定できる照明とする
			float intensity = static_cast<float>(DMXData[3]) / 255.0f;
			GetValueRegistry()->SetValue("intensity", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &intensity, sizeof(float));
			
			// Pan
			float Pan = 2.0f * 3.1415f * static_cast<float>(DMXData[4]) / 255.0f;
			GetValueRegistry()->SetValue("pan", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Pan, sizeof(float));
			
			// Tilt
			float Tilt = 2.0f * 3.1415f * static_cast<float>(DMXData[5]) / 255.0f;
			GetValueRegistry()->SetValue("tilt", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Tilt, sizeof(float));

			// Angle
			float Angle = 90.0f * static_cast<float>(DMXData[6]) / 255.0f;
			GetValueRegistry()->SetValue("angle", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Angle, sizeof(float));

			// Height
			// 50mまで届くライトとする
			float Height = 50.0f * static_cast<float>(DMXData[7]) / 255.0f;
			GetValueRegistry()->SetValue("height", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &Height, sizeof(float));
		}
	}
#endif // USE_NETWORK

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
		m_LightGeomObject = std::make_shared<object::C3DObject>();

		// PassName
		m_LightObject->AddPassName("GBufferLightPass");
		m_LightGeomObject->AddPassName("MainResultPass");

		// TextureList
		const auto& RenderPass = pGraphicsAPI->FindOffScreenRenderPass("GBufferGenPass");
		if (!RenderPass) return false;

		const auto& TextureList = RenderPass->GetFrameTextureList();
		if (TextureList.size() != 5) return false;

		for (const auto& Texture : TextureList)
		{
			m_LightObject->GetTextureSet()->AddFrameTexture(Texture);
			m_LightGeomObject->GetTextureSet()->AddFrameTexture(Texture);
		}

		// Mesh & Material
		for (const auto& MaterialFrame : m_Loader->GetTargetMaterialFrameSet())
		{
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_FRONT);
			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_ADDITIVE);

			// 他のライトが描画できなくなるので書き込まない
			Material->SetEnabledZWrite(false);

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
			const auto& Material = MaterialFrame->CreateMaterial(pGraphicsAPI, graphics::ECullMode::CULL_NONE);
			Material->SetBlendType(graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA);

			// 他のライトが描画できなくなるので書き込まない
			Material->SetEnabledZWrite(false);

			Material->ReplaceTextureIndex("gPositionTexture", 0);
			Material->ReplaceTextureIndex("gNormalTexture", 1);
			Material->ReplaceTextureIndex("gAlbedoTexture", 2);
			Material->ReplaceTextureIndex("gDepthTexture", 3);
			Material->ReplaceTextureIndex("gCustomParam0Texture", 4);

			// BoardかSphereかをライトタイプで変えるようにするとライトクラスが1つに統一できるかも？
			if (!m_LightGeomObject->AddPresetSimply(pGraphicsAPI, pPhysicsEngine, graphics::CPresetPrimitive::CreateCylinder(pGraphicsAPI), graphics::EPresetPrimitiveType::CYLINDER, Material)) return false;

			m_MaterialList.push_back(Material);

			// 1つ分しか見ない
			break;
		}

		// 生成
		if (!m_LightObject->Create(pGraphicsAPI, pPhysicsEngine)) return false;
		if (!m_LightGeomObject->Create(pGraphicsAPI, pPhysicsEngine)) return false;

		return true;
	}
}