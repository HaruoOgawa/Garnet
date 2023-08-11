#pragma once
#include <vector>
#include <memory>

#include "../Interface/IGraphicsAPI.h"
#include "../Interface/IResource.h"
#include "../Object/C3DObject.h"

namespace file { class CFile; }

namespace resource
{
	class CLoadWorker
	{
		ELoadStatus m_Status;

		std::shared_ptr<object::C3DObject> m_LoadingBar;
		std::shared_ptr<file::CFile> m_VertexShader; // ‚ ‚Æ‚ÅSimple3DObject‚ÉˆÚ“®‚·‚é
		std::shared_ptr<file::CFile> m_FragmentShader;

		std::vector<std::shared_ptr<resource::IResource>> m_FirstLoadResourceList;
		std::vector<std::shared_ptr<resource::IResource>> m_RuntimeLoadResourceList;
	public:
		CLoadWorker(api::IGraphicsAPI* pGraphicsAPI);
		virtual ~CLoadWorker();

		bool Update();
		bool Draw(api::IGraphicsAPI* pGraphicsAPI, bool IsDepthPass, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);

		bool IsLoaded();

		void AddFirstLoadResource(const std::shared_ptr<resource::IResource>& Resource);
		void AddRuntimeLoadResource(const std::shared_ptr<resource::IResource>& Resource);
	};
}