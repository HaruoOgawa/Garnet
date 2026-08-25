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
	struct SReduceBuf
	{
		std::string DstPass = std::string();
		std::string SrcPass = std::string();
	};

	class CPostProcessBloom : public IPostProcessFilter, public scriptable::CValueRegistry
	{
		std::string m_TargetPassName;

		std::vector<std::tuple<SReduceBuf, SReduceBuf, SReduceBuf>> m_ReduceBufList;

		std::shared_ptr<graphics::CFrameRenderer> m_BrightFrameRenderer;
		
		std::vector<std::tuple<std::shared_ptr<graphics::CFrameRenderer>, std::shared_ptr<graphics::CFrameRenderer>, std::shared_ptr<graphics::CFrameRenderer>>> m_ReduceFrameRendererList;

		std::shared_ptr<graphics::CFrameRenderer> m_BloomMixPassRenderer;
	public:
		CPostProcessBloom(const std::string& TargetPassName);
		virtual ~CPostProcessBloom();

		bool Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker) override;

		bool Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState) override;

		bool Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection, 
			const std::shared_ptr<graphics::CDrawInfo>& DrawInfo) override;
	};
}