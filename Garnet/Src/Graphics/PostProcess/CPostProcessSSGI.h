#pragma once

#include <memory>
#include <vector>
#include <tuple>
#include <Interface/IGraphicsAPI.h>
#include <Interface/IPostProcessFilter.h>
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
	class CPostProcessSSGI : public IPostProcessFilter, public scriptable::CValueRegistry
	{
		std::string m_TargetPassName;

		std::shared_ptr<graphics::CFrameRenderer> m_SSGIMainFrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_Reduce2x2FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_Reduce4x4FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_BilateralXBlur4x4FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_BilateralYBlur4x4FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_UpSampling2x2FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_BilateralXBlur2x2FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_BilateralYBlur2x2FrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_UpSamplingOriginFrameRenderer;
		std::shared_ptr<graphics::CFrameRenderer> m_TemporalFrameRenderer;
	public:
		CPostProcessSSGI(const std::string& TargetPassName);
		virtual ~CPostProcessSSGI();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState) override;

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
	};
}