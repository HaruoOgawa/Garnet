#include "CLoadWorker.h"
#include "CFile.h"

namespace resource
{
	CLoadWorker::CLoadWorker(api::IGraphicsAPI* pGraphicsAPI):
		m_Status(ELoadStatus::None),
		m_FirstResourceCount(0),
		m_Alpha(1.0f),
		m_LoadingBar(std::make_shared<object::C3DObject>("", "ShadowPass")),
		m_VertexShader(std::make_shared<resource::CFile>("Resources\\Shaders\\loadingbar" + pGraphicsAPI->GetVertexShaderExtension())),
		m_FragmentShader(std::make_shared<resource::CFile>("Resources\\Shaders\\loadingbar" + pGraphicsAPI->GetFragmentShaderExtension()))
	{
		m_VertexShader->Load();
		m_FragmentShader->Load();
	}

	CLoadWorker::~CLoadWorker()
	{
		m_FirstLoadResourceList.clear();
		m_RuntimeLoadResourceList.clear();
	}

	bool CLoadWorker::Create(api::IGraphicsAPI* pGraphicsAPI)
	{
		// MATERIAL
		std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
		createInfo->SetVertexShaderCode(m_VertexShader->GetData());
		createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
		auto Material = pGraphicsAPI->CreateMaterial(createInfo, 1);

		auto UniforBuffer = createInfo->CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });
		UniforBuffer->AddData("rate", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
		UniforBuffer->AddData("time", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
		UniforBuffer->AddData("alpha", &m_Alpha, sizeof(float), 0);
		UniforBuffer->AddData("pad", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);

		Material->AddShaderBuffer(UniforBuffer);

		Material->SetEnabledZTest(false);
		Material->SetCullMode(graphics::ECullMode::CULL_NONE);

		m_LoadingBar->AddMaterial(Material);

		// MESH
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<renderer::CRendererCreateInfo> rendererCreateInfo = graphics::CPresetPrimitive::CreateBoard();

		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(rendererCreateInfo, 0);
		Mesh->AddPrimitive(Primitive);
		m_LoadingBar->AddMesh(Mesh);

		// NODE
		std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0);
		m_LoadingBar->AddNode(Node);

		// Create関数を実行
		if (!m_LoadingBar->Create(pGraphicsAPI, nullptr)) return false;

		return true;
	}

	bool CLoadWorker::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		// 初期化
		if (m_Status == ELoadStatus::None)
		{
			if (!m_VertexShader->IsLoaded() || !m_FragmentShader->IsLoaded()) return true;

			if (!Create(pGraphicsAPI)) return false;

			m_FirstResourceCount = static_cast<int>(m_FirstLoadResourceList.size()); // 初回ロードのリソース数を取得

			if (m_FirstResourceCount > 0)
			{
				m_Status = ELoadStatus::Loading;
			}
			else
			{
				// 初回ロードリソースがない場合は即ロード完了にする
				m_Status = ELoadStatus::Loaded;
			}
		}

		// ローディングバー
		if (m_LoadingBar)
		{
			if (!m_LoadingBar->Update(pGraphicsAPI, 0.0f)) return false;
		}

		// ローディング
		if (m_Status != ELoadStatus::Loaded) // 初回リソースのロード
		{
			for (auto& Resource : m_FirstLoadResourceList)
			{
				if (!Resource->Update(pGraphicsAPI)) return false;

				switch (Resource->GetStatus())
				{
				case resource::ELoadStatus::None:
					if (!Resource->Load()) return false;
					return true;

				case resource::ELoadStatus::Loading:
					return true;

				case resource::ELoadStatus::Loaded:
				{
					m_FirstLoadResourceList.erase(m_FirstLoadResourceList.begin());
					m_FirstLoadResourceList.shrink_to_fit();
				}
				return true;

				default:
					break;
				}
			}

			m_Status = ELoadStatus::Loaded;
		}
		else // ランタイムリソースのロード
		{
			for (auto& Resource : m_RuntimeLoadResourceList)
			{
				switch (Resource->GetStatus())
				{
				case resource::ELoadStatus::None:
					if (!Resource->Load()) return false;
					return true;

				case resource::ELoadStatus::Loading:
					if (!Resource->Update(pGraphicsAPI)) return false;
					return true;

				case resource::ELoadStatus::Loaded:
				{
					m_RuntimeLoadResourceList.erase(m_RuntimeLoadResourceList.begin());
					m_RuntimeLoadResourceList.shrink_to_fit();

					return true;
				}

				default:
					break;
				}
			}
		}

		return true;
	}

	bool CLoadWorker::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if(m_Status == ELoadStatus::Loaded) return true;

		if (m_Status == ELoadStatus::Loading && m_LoadingBar)
		{
			float rate = 1.0f - (static_cast<float>(m_FirstLoadResourceList.size()) / m_FirstResourceCount);
			m_LoadingBar->GetMaterialList()[0]->SetUniformValue("rate", &glm::vec1(rate)[0], sizeof(float));
			m_LoadingBar->GetMaterialList()[0]->SetUniformValue("alpha", &m_Alpha, sizeof(float));

			if (!m_LoadingBar->Draw(IsDepthPass, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool CLoadWorker::IsLoaded()
	{
		return (m_Status == ELoadStatus::Loaded);
	}

	void CLoadWorker::AddFirstLoadResource(const std::shared_ptr<resource::IResource>& Resource)
	{
		m_FirstLoadResourceList.push_back(Resource);
	}

	void CLoadWorker::AddRuntimeLoadResource(const std::shared_ptr<resource::IResource>& Resource)
	{
		m_RuntimeLoadResourceList.push_back(Resource);
	}
}