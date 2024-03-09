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
		m_VertexShader->LoadImmediate();
		m_FragmentShader->LoadImmediate();
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
		auto Material = pGraphicsAPI->CreateMaterial(createInfo, 1, graphics::ECullMode::CULL_BACK);

		auto UniforBuffer = createInfo->CreateUniformBuffer("UniformBufferObject", {graphics::SBindingLayout("UniformBufferObject", 0, false)});
		UniforBuffer->AddData("rate", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
		UniforBuffer->AddData("time", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);
		UniforBuffer->AddData("alpha", &m_Alpha, sizeof(float), 0);
		UniforBuffer->AddData("pad", &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0);

		Material->AddShaderBuffer(UniforBuffer);

		Material->SetEnabledZTest(false);
		Material->SetCullMode(graphics::ECullMode::CULL_NONE);

		// Create関数を実行
		if (!m_LoadingBar->CreateSimply(pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), Material, nullptr)) return false;

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
			if (!m_LoadingBar->Update(pGraphicsAPI, nullptr, 0.0f)) return false;
		}

		// ローディング
		if (m_Status != ELoadStatus::Loaded) // 初回リソースのロード
		{
			for (auto& Resource : m_FirstLoadResourceList)
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

			if (!m_LoadingBar->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
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