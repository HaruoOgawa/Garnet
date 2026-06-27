#include "CPostProcessSSGI.h"

namespace graphics
{
	CPostProcessSSGI::CPostProcessSSGI(const std::string& TargetPassName):
		CValueRegistry("PostProcessSSGIRegistry"),
		m_TargetPassName(TargetPassName)
	{
	}

	CPostProcessSSGI::~CPostProcessSSGI()
	{
	}

	bool CPostProcessSSGI::Initialize(api::IGraphicsAPI* pGraphicsAPI, resource::CLoadWorker* pLoadWorker)
	{
		return true;
	}

	bool CPostProcessSSGI::Update(api::IGraphicsAPI* pGraphicsAPI, physics::IPhysicsEngine* pPhysicsEngine, resource::CLoadWorker* pLoadWorker,
		const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo, const std::shared_ptr<input::CInputState>& InputState)
	{
		return true;
	}

	bool CPostProcessSSGI::Draw(api::IGraphicsAPI* pGraphicsAPI, const std::shared_ptr<camera::CCamera>& Camera,
		const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		return true;
	}
}