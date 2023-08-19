#include "CScriptScene.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "../Object/C3DObject.h"
#include "../File/CFile.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>
#include "../Graphics/CDrawInfo.h"
#include "../Camera/CCamera.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_InstanceCount(512),

		m_TestObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_DepthVertex(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetVertexShaderExtension())),
		m_DepthFragment(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_VertexShader(std::make_shared<file::CFile>("Resources\\Shaders\\sample" + pGraphicsAPI->GetVertexShaderExtension())),
		m_FragmentShader(std::make_shared<file::CFile>("Resources\\Shaders\\sample" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(m_DepthVertex);
		pLoadWorker->AddFirstLoadResource(m_DepthFragment);
		pLoadWorker->AddFirstLoadResource(m_VertexShader);
		pLoadWorker->AddFirstLoadResource(m_FragmentShader);
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// TestObj
		{
			// MATERIAL
			{
				std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
				createInfo->SetVertexShaderCode(m_VertexShader->GetData());
				createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
				auto Material0 = pGraphicsAPI->CreateMaterial(createInfo);

				// UBO, TEXTURE
				{
					auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0) });

					UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
					UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
					UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
					UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

					Material0->AddShaderBuffer(UniformBuffer);

					//
					auto SSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("TestBufferObject", 1) });

					std::vector<float> InitData;
					for (int i = 0; i < m_InstanceCount; i++)
					{
						float w = 10.0;
						float f_id = static_cast<float>(i);

						glm::vec4 offset = glm::vec4(
							w * (rand(glm::vec2(f_id, 55.5)) * 2.0 - 1.0),
							w * (rand(glm::vec2(943.22, f_id)) * 2.0 - 1.0),
							w * (rand(glm::vec2(f_id + 11.111, f_id + 456.123)) * 2.0 - 1.0),
							1.0f
						);
						glm::vec4 color = glm::vec4(
							rand(glm::vec2(f_id, 55.5)),
							rand(glm::vec2(943.22, f_id)),
							rand(glm::vec2(f_id + 11.111, f_id + 456.123)),
							1.0f
						);

						InitData.push_back(offset.x); InitData.push_back(offset.y); InitData.push_back(offset.z); InitData.push_back(offset.w);
						InitData.push_back(color.x); InitData.push_back(color.y); InitData.push_back(color.z); InitData.push_back(color.w);
					}

					SSBO->AddData("rw_TBO", &InitData[0], sizeof(float) * static_cast<int>(InitData.size()), 1);

					Material0->AddShaderBuffer(SSBO);
				}

				m_TestObject->AddMaterial(Material0);
			}

			// MESH
			{
				std::shared_ptr<graphics::CMesh> Mesh0 = std::make_shared<graphics::CMesh>();

				std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();
				if (!graphics::CPresetPrimitive::CreateBox(createInfo)) return false;
				createInfo->SetInstanceDrawCount(m_InstanceCount);

				std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(createInfo, 0);
				Mesh0->AddPrimitive(Primitive);
				m_TestObject->AddMesh(Mesh0);
			}

			// NODE
			{
				std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_TestObject->GetMeshList(), m_TestObject->GetMaterialList());
				Node->SetMeshIndex(0);
				Node->SetPos(glm::vec3(0.0f, 0.0f, -1.0f));
				m_TestObject->AddNode(Node);
			}

			// CreateŠÖ”‚ðŽÀs
			if (!m_TestObject->Create(pGraphicsAPI, m_DepthVertex, m_DepthFragment)) return false;
		}

		return true;
	}

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load(pGraphicsAPI)) return false;
			m_IsLoaded = true;
		}

		if (m_TestObject)
		{
			if (!m_TestObject->Update()) return false;
		}

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (m_TestObject)
		{
			if (!m_TestObject->Draw(IsDepthPass, SecondsTime, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}