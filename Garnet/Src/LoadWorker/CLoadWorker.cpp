#include "CLoadWorker.h"
#include "CFile.h"
#include "../AppCore/CAppCore.h"
#include "../Message/Console.h"

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
		m_InitialResourceList.push_back(m_VertexShader);
		m_InitialResourceList.push_back(m_FragmentShader);
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
		if (!m_LoadingBar->CreatePresetSimply(pGraphicsAPI, nullptr, graphics::CPresetPrimitive::CreateBoard(pGraphicsAPI), graphics::EPresetPrimitiveType::BOARD, Material, nullptr)) return false;

		return true;
	}

	bool CLoadWorker::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore)
	{
		// 初回ロード
		bool ExistError = false;
		if (CheckFirstLoading(ExistError, pGraphicsAPI, pPhysicsEngine, pAppCore)) return true;
		if (ExistError)
		{
			Console::Log("[Error] Failed to FirstLoading\n");
			return false;
		}

		// ローディングバー
		if (m_LoadingBar)
		{
			if (!m_LoadingBar->Update(pGraphicsAPI, nullptr, 0.0f)) return false;
		}

		// ローディング
		if (!LoadResourceList(pGraphicsAPI, pPhysicsEngine, pAppCore)) return false;
		
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

			if (!m_LoadingBar->Draw(pGraphicsAPI, IsDepthPass, false, Camera, Projection, DrawInfo)) return false;
		}

		return true;
	}

	bool CLoadWorker::CheckFirstLoading(bool& ExistError, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore)
	{
		ExistError = false;

		// 初期化
		if (m_Status == ELoadStatus::None)
		{
			if (CheckInitialResource(pGraphicsAPI, pPhysicsEngine, pAppCore)) return true;
			if (!InitLoadStatus(pGraphicsAPI, pAppCore)) ExistError = true;

			// 起動準備完了イベントコールバックの呼び出し
			if (!pAppCore->OnStartup()) return false;
		}
		else if (m_Status == ELoadStatus::Loading)
		{
			if (m_LoadResourceList.empty())
			{
				m_Status = ELoadStatus::Loaded;

				// 初回メインロード完了イベントコールバックの呼び出し
				if(!pAppCore->OnLoaded()) ExistError = true;
			}
		}

		return false;
	}

	bool CLoadWorker::CheckInitialResource(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore)
	{
		for (auto& Resource : m_InitialResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				// ファイルのバイナリが実行ファイルに埋め込まれていないかチェックする
				if (FindEmbeddedBinary(pGraphicsAPI, pPhysicsEngine, Resource, pAppCore)) return true;

				// 通常通りロードする
				if (!Resource->Load()) return false;

				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, this, pAppCore->GetApp().get())) return false;
				return true;

			case resource::ELoadStatus::Loaded:
			{
				m_InitialResourceList.erase(m_InitialResourceList.begin());
			}
			return true;

			default:
				break;
			}
		}
		
		return false;
	}

	bool CLoadWorker::InitLoadStatus(api::IGraphicsAPI* pGraphicsAPI, app::CAppCore* pAppCore)
	{
		if (!Create(pGraphicsAPI)) return false;

		m_FirstResourceCount = static_cast<int>(m_LoadResourceList.size()); // 初回ロードのリソース数を取得

		m_Status = ELoadStatus::Loading;

		return true;
	}

	bool CLoadWorker::LoadResourceList(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore)
	{
		// ローディング
		for (auto& Resource : m_LoadResourceList)
		{
			switch (Resource->GetStatus())
			{
			case resource::ELoadStatus::None:
				// ファイルのバイナリが実行ファイルに埋め込まれていないかチェックする
				if (FindEmbeddedBinary(pGraphicsAPI, pPhysicsEngine, Resource, pAppCore)) return true;

				// 通常通りロードする
				if (!Resource->Load()) return false;
				
				return true;

			case resource::ELoadStatus::Loading:
				if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, this, pAppCore->GetApp().get())) return false;
				return true;

			case resource::ELoadStatus::Loaded:
				m_LoadResourceList.erase(m_LoadResourceList.begin());
				return true;

			case resource::ELoadStatus::AssertError:
				{
					// エラー表示通知を行う(強制終了しないタイプのエラーなので通知)
					pAppCore->OnAssertError(Resource->GetAssertedErrorMessage());

					Resource->SetLoadStatus(resource::ELoadStatus::Loaded);

					return true;
				}

			default:
				break;
			}
		}

		return true;
	}

	bool CLoadWorker::IsLoaded()
	{
		return (m_Status == ELoadStatus::Loaded);
	}

	void CLoadWorker::AddScene(const std::shared_ptr<CSceneLoader>& SceneLoader)
	{
		m_InitialResourceList.push_back(SceneLoader);
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

	bool CLoadWorker::FindEmbeddedBinary(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, std::shared_ptr<resource::IResource>& Resource, app::CAppCore* pAppCore)
	{
		std::string Key = Resource->GetFilename();
		size_t Index = Key.find("\\");
		while (Index != -1)
		{
			Key.replace(Index, 1, "/");

			Index = Key.find("\\");
		}

		auto Binary = pAppCore->GetApp()->GetEmbeddedBinary(Key);

		const bool Exist = (!Binary.empty());

		if (Exist)
		{
			Resource->SetDataWithLoaded(Binary);

			// Updateだけ一回実行しておく
			if (!Resource->Update(pGraphicsAPI, pPhysicsEngine, this, pAppCore->GetApp().get())) return false;
		}

		return Exist;
	}

	const std::shared_ptr<CResourceManager>& CLoadWorker::GetResourceManager() const
	{
		return m_ResourceManager;
	}
}