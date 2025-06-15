#ifdef USE_RTXGI
#include "CRTXGIController.h"

namespace graphics
{
	CRTXGIController::CRTXGIController()
	{
	}
	
	CRTXGIController::~CRTXGIController()
	{
	}

	bool CRTXGIController::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CRTXGIController::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CRTXGIController::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}
}
#endif // USE_RTXGI