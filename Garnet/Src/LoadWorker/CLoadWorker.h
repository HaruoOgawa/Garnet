#pragma once
#include <vector>
#include <unordered_map>
#include <memory>

#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IResource.h"
#include "../Object/C3DObject.h"
#include "CResourceManager.h"
#include "CSceneLoader.h"
#include "CMaterialFrameLoader.h"
#include "CTimelineClipLoader.h"

namespace file { class CFile; }
namespace app { class CAppCore; }

namespace resource
{
	class CSceneLoader;

	class CLoadWorker
	{
		ELoadStatus m_Status;

		std::shared_ptr<CResourceManager> m_ResourceManager;

		int m_FirstResourceCount;
		float m_Alpha;

		std::shared_ptr<object::C3DObject> m_LoadingBar;
		std::shared_ptr<resource::CFile> m_VertexShader; // ‚ ‚Æ‚ÅSimple3DObject‚ÉˆÚ“®‚·‚é
		std::shared_ptr<resource::CFile> m_FragmentShader;

		std::vector<std::shared_ptr<resource::IResource>> m_InitialResourceList;
		std::vector<std::shared_ptr<resource::IResource>> m_LoadResourceList;
	private:
		bool Create(api::IGraphicsAPI* pGraphicsAPI);

		bool CheckFirstLoading(bool& ExistError, api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore);
		bool CheckInitialResource(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);
		bool InitLoadStatus(api::IGraphicsAPI* pGraphicsAPI, app::CAppCore* pAppCore);
		
		bool LoadResourceList(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine);

		std::shared_ptr<resource::IResource> GetLoadingResource(const std::string& Filename) const;
	public:
		CLoadWorker(api::IGraphicsAPI* pGraphicsAPI);
		virtual ~CLoadWorker();

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, app::CAppCore* pAppCore);
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool IsLoaded();

		void AddScene(const std::shared_ptr<CSceneLoader>& SceneLoader);

		void AddLoadResource(const std::shared_ptr<resource::IResource>& Resource);

		const std::shared_ptr<CResourceManager>& GetResourceManager() const;
	};
}