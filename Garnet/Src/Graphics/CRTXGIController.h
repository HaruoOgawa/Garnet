#pragma once

#ifdef USE_RTXGI
namespace api { class IGraphicsAPI; }

namespace graphics
{
	class CRTXGIController
	{
	public:
		CRTXGIController();
		virtual ~CRTXGIController();

		virtual bool Initialize(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool Update(api::IGraphicsAPI* pGraphicsAPI);
		virtual bool Draw(api::IGraphicsAPI* pGraphicsAPI);
	};
}
#endif // USE_RTXGI