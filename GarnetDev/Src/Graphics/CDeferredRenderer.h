#pragma once

#include <memory>
#include <Interface/IGraphicsAPI.h>
#include <Object/C3DObject.h>

namespace resource { class CLoadWorker; }
namespace api { class IGraphicsAPI; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace graphics { class CDrawInfo; }

namespace graphics
{
	class CDeferredRenderer
	{
		bool m_IsLoaded;

		api::IGraphicsAPI* m_pGraphicsAPI;

		std::shared_ptr<graphics::CMaterialFrame> m_MRTMF;

		std::shared_ptr<object::C3DObject> m_RenderBoard;

	private:
		bool Load();
	public:
		CDeferredRenderer(api::IGraphicsAPI* pGraphicsAPI);
		virtual ~CDeferredRenderer();

		bool Create(resource::CLoadWorker* pLoadWorker);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);

		bool Draw(const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}