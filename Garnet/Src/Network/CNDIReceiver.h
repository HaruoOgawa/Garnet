#pragma once

#ifdef USE_NETWORK

#include <vector>

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

	private:
		// YUVÅ®RGBïœä∑ÅiBT.601Åj
		static void YUV2RGB(unsigned char Y, unsigned char U, unsigned char V, unsigned char& R, unsigned char& G, unsigned char& B);

		// UYVY Å® BGRAïœä∑
		static void ConvertUYVYtoBGRA(const unsigned char* uyvy, int width, int height, int stride, std::vector<unsigned char>& bgra);

	public:
		CNDIReceiver();
		virtual ~CNDIReceiver();

		bool Initialize();

		bool Update(app::CApp* pApp);
	};
}
#endif // USE_NETWORK