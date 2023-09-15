#include "CScriptScene.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "../Object/C3DObject.h"
#include "../File/CFile.h"
#include "../Debug/Message/Console.h"
#include "../GLTF/CGLTFImporter.h"
#include <glm/glm.hpp>
#include "../Graphics/CDrawInfo.h"
#include "../Camera/CCamera.h"
#include "../Interface/IGPGPUHandler.h"

namespace scene
{
	CScriptScene::CScriptScene(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker):
		m_InstanceCount(512),

		m_GPGPUHandler(nullptr),

		m_TestObject(std::make_shared<object::C3DObject>("", "ShadowPass")),

		m_DepthVertex(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetVertexShaderExtension())),
		m_DepthFragment(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_VertexShader(std::make_shared<file::CFile>("Resources\\Shaders\\gpgpu_sample" + pGraphicsAPI->GetVertexShaderExtension())),
		m_FragmentShader(std::make_shared<file::CFile>("Resources\\Shaders\\gpgpu_sample" + pGraphicsAPI->GetFragmentShaderExtension())),
		m_ComputeShader(std::make_shared<file::CFile>("Resources\\Shaders\\gpgpu_sample" + pGraphicsAPI->GetComputeShaderExtension())),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(m_DepthVertex);
		pLoadWorker->AddFirstLoadResource(m_DepthFragment);
		pLoadWorker->AddFirstLoadResource(m_VertexShader);
		pLoadWorker->AddFirstLoadResource(m_FragmentShader);
		pLoadWorker->AddFirstLoadResource(m_ComputeShader);
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		//
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
			InitData.push_back(0.0f);
			InitData.push_back(0.0f);
			InitData.push_back(0.0f);
			InitData.push_back(0.0f);
		}

		// Compute Buffer
		{
			std::shared_ptr<graphics::CMaterialCreateInfo> computeCreateInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			computeCreateInfo->SetComputeShaderCode(m_ComputeShader->GetData());

			auto ComputeMaterial = pGraphicsAPI->CreateMaterial(computeCreateInfo);

			auto ParamUBO = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("ParamUBO", 0, false) });
			ParamUBO->AddData("time", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
			ParamUBO->AddData("deltaTime", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
			ParamUBO->AddData("pad0", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
			ParamUBO->AddData("pad1", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);

			ComputeMaterial->AddShaderBuffer(ParamUBO);

			auto ReadSSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("ReadOnlyTestBufferObject", 1, false) });
			ReadSSBO->AddData("r_TBO", &InitData[0], sizeof(float) * static_cast<int>(InitData.size()), 1);

			ComputeMaterial->AddShaderBuffer(ReadSSBO);

			auto WriteSSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("WriteOnlyTestBufferObject", 2, true) });
			WriteSSBO->AddData("w_TBO", &InitData[0], sizeof(float) * static_cast<int>(InitData.size()), 2);

			ComputeMaterial->AddShaderBuffer(WriteSSBO);

			//
			m_GPGPUHandler = pGraphicsAPI->CreateGPGPUHandler(ComputeMaterial);
			if (!m_GPGPUHandler->Create()) return false;
		}

		// TestObj
		{
			// MATERIAL
			{
				std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
				createInfo->SetVertexShaderCode(m_VertexShader->GetData());
				createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
				auto Material0 = pGraphicsAPI->CreateMaterial(createInfo);

				auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });

				UniformBuffer->AddData("model", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("view", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("proj", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);
				UniformBuffer->AddData("lightVPMat", &glm::mat4(1.0f)[0][0], sizeof(glm::mat4), 0);

				Material0->AddShaderBuffer(UniformBuffer);
				auto SSBO = graphics::CMaterialCreateInfo::CreateShaderStorageBuffer({ graphics::SBindingLayout("TestBufferObject", 1, false) });
				
				SSBO->AddData("rw_TBO", &InitData[0], sizeof(float) * static_cast<int>(InitData.size()), 1);
				SSBO->SetSharedBufferParam({ true, 2, m_GPGPUHandler->GetComputeMaterial() });

				Material0->AddShaderBuffer(SSBO);

				Material0->SetBlendType(graphics::EBlendType::BLEND_TYPE_TRANSPARENT_ALPHA);

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

	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
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

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded || !m_GPGPUHandler) return true;

		if (!m_GPGPUHandler->Dispatch(glm::ivec3(m_InstanceCount / 256, 1, 1), Camera, Projection, DrawInfo)) return false;

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (m_TestObject)
		{
			if (!m_TestObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}