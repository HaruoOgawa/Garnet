#pragma once

#ifdef USE_NETWORK
namespace app { class CApp; }

struct NDIlib_find_instance_type;
struct NDIlib_recv_instance_type;

namespace network
{
	class CNDIReceiver
	{
		NDIlib_find_instance_type* m_NDI_find;
		NDIlib_recv_instance_type* m_NDI_recv;

		bool m_Connected;
	public:
		CNDIReceiver();
		virtual ~CNDIReceiver();

		bool Initialize();

		bool Update(app::CApp* pApp);
	};
}
#endif // USE_NETWORK