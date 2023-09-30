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
		m_glTFObject(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_glTFData(std::make_shared<file::CFile>("Resources\\Models\\Sponza\\glTF\\Sponza.glb")),
		//m_glTFData(std::make_shared<file::CFile>("Resources\\Models\\DamagedHelmet\\glTF-Binary\\DamagedHelmet.glb")),

		m_Cube0(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_back_0.jpg")),
		m_Cube1(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_bottom_0.jpg")),
		m_Cube2(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_front_0.jpg")),
		m_Cube3(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_left_0.jpg")),
		m_Cube4(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_right_0.jpg")),
		m_Cube5(std::make_shared<file::CFile>("Resources\\Cubemaps\\environment\\environment_top_0.jpg")),

		m_DepthVertex(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetVertexShaderExtension())),
		m_DepthFragment(std::make_shared<file::CFile>("Resources\\Shaders\\depth" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_VertexShader(std::make_shared<file::CFile>("Resources\\Shaders\\pbr" + pGraphicsAPI->GetVertexShaderExtension())),
		m_FragmentShader(std::make_shared<file::CFile>("Resources\\Shaders\\pbr" + pGraphicsAPI->GetFragmentShaderExtension())),

		m_IsLoaded(false)
	{
		pLoadWorker->AddFirstLoadResource(m_DepthVertex);
		pLoadWorker->AddFirstLoadResource(m_DepthFragment);
		pLoadWorker->AddFirstLoadResource(m_glTFData);
		pLoadWorker->AddFirstLoadResource(m_VertexShader);
		pLoadWorker->AddFirstLoadResource(m_FragmentShader);
		pLoadWorker->AddFirstLoadResource(m_Cube0);
		pLoadWorker->AddFirstLoadResource(m_Cube1);
		pLoadWorker->AddFirstLoadResource(m_Cube2);
		pLoadWorker->AddFirstLoadResource(m_Cube3);
		pLoadWorker->AddFirstLoadResource(m_Cube4);
		pLoadWorker->AddFirstLoadResource(m_Cube5);
	}

	CScriptScene::~CScriptScene()
	{
		m_IsLoaded = false;
	}

	bool CScriptScene::Load(api::IGraphicsAPI* pGraphicsAPI)
	{
		// Cubemap
		std::vector<std::shared_ptr<graphics::CTexture>> CubeTexList;
		{
			std::vector<std::vector<unsigned char>> CubeDataList;
			CubeDataList.push_back(m_Cube0->GetData());
			CubeDataList.push_back(m_Cube1->GetData());
			CubeDataList.push_back(m_Cube2->GetData());
			CubeDataList.push_back(m_Cube3->GetData());
			CubeDataList.push_back(m_Cube4->GetData());
			CubeDataList.push_back(m_Cube5->GetData());

			auto CubeTex0 = pGraphicsAPI->CreateTexture(true);
			if (!CubeTex0->Create(CubeDataList)) return false;

			CubeTexList.push_back(CubeTex0);
		}

		// glTFObject
		{
			m_glTFObject->SetRot(glm::vec3(0.0f, 3.1415f * -0.5f, 0.0f));

			std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
			createInfo->SetVertexShaderCode(m_VertexShader->GetData());
			createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
			if (!gltf::CGLTFImporter::Import(pGraphicsAPI, m_glTFData->GetData(), m_glTFObject, createInfo, CubeTexList, std::vector<std::shared_ptr<graphics::CTexture>>(), m_DepthVertex, m_DepthFragment)) return false;
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

		if (m_glTFObject)
		{
			if (!m_glTFObject->Update()) return false;
		}

		return true;
	}

	bool CScriptScene::Dispatch(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (m_glTFObject)
		{
			if (!m_glTFObject->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	// Tex of FrameBuffer
	void CScriptScene::SetFrameTexture(const std::shared_ptr<graphics::CTexture>& FrameTexture)
	{
		m_FrameTextureList.push_back(FrameTexture);
	}
}