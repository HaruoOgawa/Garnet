#pragma once

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <Interface/IGraphicsAPI.h>
#include <Scriptable/CValueRegistry.h>

namespace resource { class CLoadWorker; }
namespace camera { class CCamera; }
namespace projection { class CProjection; }
namespace input { class CInputState; }
namespace physics { class IPhysicsEngine; }
namespace scene { class CSceneController; }
namespace graphics
{
	class CFrameRenderer;
	class CDrawInfo;
}

namespace graphics
{
	class CPostProcessFXAA : public scriptable::CValueRegistry
	{
		std::string m_TargetPassName;

		std::shared_ptr<graphics::CFrameRenderer> m_FXAAFrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_ResultFrameRenderer;

		std::shared_ptr<graphics::CTexture> m_TargetTexture;
	public:
		CPostProcessFXAA(const std::string& TargetPassName);
		virtual ~CPostProcessFXAA();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker);

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState);

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo);
	};
}


