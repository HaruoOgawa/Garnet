#include "CLoadWorker.h"
#include "CFile.h"

namespace resource
{
	CLoadWorker::CLoadWorker(api::IGraphicsAPI* pGraphicsAPI):
		m_Status(ELoadStatus::None),
		m_ResourceManager(std::make_shared<CResourceManager>()),
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
		m_LoadResourceList.clear();
	}

	bool CLoadWorker::Create(api::IGraphicsAPI* pGraphicsAPI)
	{
		// MATERIAL
		std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
		createInfo->SetVertexShaderCode(m_VertexShader->GetData());
		createInfo->SetFragmentShaderCode(m_FragmentShader->GetData());
		auto Material = pGraphicsAPI->CreateMaterial(createInfo, 1, graphics::ECullMode::CULL_BACK);

		auto UniforBuffer = createInfo->CreateUniformBuffer("UniformBufferObject", {graphics::SBindingLayout("UniformBufferObject", 0, false)});
		UniforBuffer->AddData("rate", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0, graphics::SUniformValueInput{});
		UniforBuffer->AddData("time", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0, graphics::SUniformValueInput{});
		UniforBuffer->AddData("alpha", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &m_Alpha, sizeof(float), 0, graphics::SUniformValueInput{});
		UniforBuffer->AddData("pad", graphics::EUniformValueType::VALUE_TYPE_FLOAT, &glm::vec1(0.0f)[0], sizeof(glm::vec1), 0, graphics::SUniformValueInput{});

		Material->AddShaderBuffer(UniforBuffer);

		Material->SetDepthFunc(graphics::EDepthFunc::Always);
		Material->SetCullMode(graphics::ECullMode::CULL_NONE);

		// Create関数を実行
		if (!m_LoadingBar->CreateSimply(pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), Material, nullptr)) return false;

		return true;
	}

	bool CLoadWorker::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		// 初期化
		if (!InitLoadStatus(pGraphicsAPI)) return false;
		
		// ローディングバー
		if (m_LoadingBar)
		{
			if (!m_LoadingBar->Update(pGraphicsAPI, nullptr, 0.0f)) return false;
		}

		// ローディング
		if (!LoadResourceList(pGraphicsAPI, pPhysicsEngine)) return false;
		
		return true;
	}

	bool CLoadWorker::Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if(m_Status == ELoadStatus::Loaded) return true;

		if (m_Status == ELoadStatus::Loading && m_LoadingBar)
		{
			float rate = 1.0f - (static_cast<float>(m_LoadResourceList.size()) / m_FirstResourceCount);
			m_LoadingBar->GetMaterialList()[0]->SetUniformValue("rate", &glm::vec1(rate)[0], sizeof(float));
			m_LoadingBar->GetMaterialList()[0]->SetUniformValue("alpha", &m_Alpha, sizeof(float));

			if (!m_LoadingBar->Draw(IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool CLoadWorker::InitLoadStatus(api::IGraphicsAPI* pGraphicsAPI)
	{
		// 初期化
		if (m_Status == ELoadStatus::None)
		{
			if (!m_VertexShader->IsLoaded() || !m_FragmentShader->IsLoaded()) return true;

			if (!Create(pGraphicsAPI)) return false;

			m_FirstResourceCount = static_cast<int>(m_LoadResourceList.size()); // 初回ロードのリソース数を取得

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

		return true;
	}

	bool CLoadWorker::LoadResourceList(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine)
	{
		// ローディング
		for (auto& Resource : m_LoadResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				if (!Resource->Load()) return false;
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, m_ResourceManager)) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				m_LoadResourceList.erase(m_LoadResourceList.begin());
			}
			return true;

			default:
				break;
			}
		}

		m_Status = ELoadStatus::Loaded;

		return true;
	}

	bool CLoadWorker::IsLoaded()
	{
		return (m_Status == ELoadStatus::Loaded);
	}

	void CLoadWorker::AddLoadResource(const std::shared_ptr<resource::IResource>& Resource)
	{
		auto LoadingResource = GetLoadingResource(Resource->GetFilename());

		if (LoadingResource)
		{
			// リソースにターゲットの参照だけを追加する
			LoadingResource->AddReference(Resource);
		}
		else
		{
			// mapに新規追加する
			m_LoadResourceList.push_back(Resource);
		}
	}

	std::shared_ptr<resource::IResource> CLoadWorker::GetLoadingResource(const std::string& Filename) const
	{
		for (const auto& Resouce : m_LoadResourceList)
		{
			if (Filename == Resouce->GetFilename())
			{
				return Resouce;
			}
		}

		return nullptr;
	}

	const std::shared_ptr<CResourceManager>& CLoadWorker::GetResourceManager() const
	{
		return m_ResourceManager;
	}
}