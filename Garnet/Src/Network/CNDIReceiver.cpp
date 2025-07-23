#ifdef USE_NETWORK
#include "CNDIReceiver.h"
#include "../AppCore/CApp.h"
#include "../Message/Console.h"
#include "../Binary/CBinaryReader.h"
#include "../GraphicsAPI/ERenderPassFormat.h"
#include <algorithm>

#include <Processing.NDI.Lib.h>
//#include <Processing.NDI.Recv.h>
//#include <Processing.NDI.Lib.cplusplus.h>

namespace network
{
	CNDIReceiver::CNDIReceiver():
		m_NDI_find(nullptr),
		m_NDI_recv(nullptr),
		m_Connected(false)
	{
	}

	CNDIReceiver::~CNDIReceiver()
	{
		NDIlib_find_destroy(m_NDI_find);
		NDIlib_recv_destroy(m_NDI_recv);
	}

	bool CNDIReceiver::Initialize()
	{
		// NDIを配信しているソースを探す ////////////////////////////////////////////
		// ファインダーを生成 
		m_NDI_find = NDIlib_find_create_v2(nullptr);
		if (!m_NDI_find) return true;

		uint32_t no_sources = 0;
		NDIlib_find_wait_for_sources(m_NDI_find, 1000);
		const NDIlib_source_t* pSources = NDIlib_find_get_current_sources(m_NDI_find, &no_sources);

		if (no_sources == 0) return true;


		// NDIの受信準備 ///////////////////////////////////////////////////////////
		// レシーバーを作成
		m_NDI_recv = NDIlib_recv_create_v3(nullptr); // 引数には詳細設定を渡す
		if (!m_NDI_recv) return true;

		// ネットワーク接続
		NDIlib_recv_connect(m_NDI_recv, &pSources[0]); // ひとまず最初のソースを使う(必要になれば名前とかポートとかを見て判断する)

		// 接続完了
		m_Connected = true;

		return true;
	}

	// YUV→RGB変換（BT.601）
	void CNDIReceiver::YUV2RGB(unsigned char Y, unsigned char U, unsigned char V, unsigned char& R, unsigned char& G, unsigned char& B)
	{
		int C = Y - 16;
		int D = U - 128;
		int E = V - 128;
		int r = (298 * C + 409 * E + 128) >> 8;
		int g = (298 * C - 100 * D - 208 * E + 128) >> 8;
		int b = (298 * C + 516 * D + 128) >> 8;
		R = static_cast<unsigned char>(std::clamp(r, 0, 255));
		G = static_cast<unsigned char>(std::clamp(g, 0, 255));
		B = static_cast<unsigned char>(std::clamp(b, 0, 255));
	}

	// UYVY → BGRA変換
	void CNDIReceiver::ConvertUYVYtoBGRA(const unsigned char* uyvy, int width, int height, int stride, std::vector<unsigned char>& bgra)
	{
		bgra.resize(width * height * 4);
		for (int y = 0; y < height; ++y)
		{
			const unsigned char* src = uyvy + y * stride;
			unsigned char* dst = bgra.data() + y * width * 4;
			for (int x = 0; x < width; x += 2)
			{
				unsigned char U = src[0];
				unsigned char Y0 = src[1];
				unsigned char V = src[2];
				unsigned char Y1 = src[3];

				unsigned char R, G, B;
				YUV2RGB(Y0, U, V, R, G, B);
				dst[0] = B; dst[1] = G; dst[2] = R; dst[3] = 255; // BGRA
				YUV2RGB(Y1, U, V, R, G, B);
				dst[4] = B; dst[5] = G; dst[6] = R; dst[7] = 255; // BGRA

				src += 4;
				dst += 8;
			}
		}
	}

	bool CNDIReceiver::Update(app::CApp* pApp)
	{
		if (!m_Connected || !m_NDI_recv) return true;

		NDIlib_video_frame_v2_t videoFrame;
		NDIlib_frame_type_e frameType = NDIlib_recv_capture_v2(m_NDI_recv, &videoFrame, nullptr, nullptr, 0);

		int Width = videoFrame.xres;
		int Height = videoFrame.yres;

		api::ERenderPassFormat Format = api::ERenderPassFormat::NONE;

		if (frameType == NDIlib_frame_type_video)
		{
			std::vector<unsigned char> pixelBuffer;

			if (videoFrame.FourCC == NDIlib_FourCC_video_type_UYVY)
			{
				ConvertUYVYtoBGRA(
					videoFrame.p_data,
					videoFrame.xres,
					videoFrame.yres,
					videoFrame.line_stride_in_bytes,
					pixelBuffer
				);
				// pixelBufferにBGRAデータが格納される
			}
			else if (videoFrame.FourCC == NDIlib_FourCC_video_type_BGRA)
			{
				// BGRAならそのままコピー
				pixelBuffer.resize(videoFrame.xres * videoFrame.yres * 4);
				for (int y = 0; y < videoFrame.yres; ++y)
				{
					const unsigned char* src = videoFrame.p_data + y * videoFrame.line_stride_in_bytes;
					unsigned char* dst = pixelBuffer.data() + y * videoFrame.xres * 4;
					std::copy(src, src + videoFrame.xres * 4, dst);
				}
			}

			// ビデオフォーマット的にはBGRAは一般的らしいのでひとまずこれを使う
			Format = api::ERenderPassFormat::COLOR_BGRA;

			NDIlib_recv_free_video_v2(m_NDI_recv, &videoFrame);

			// Appに通知
			pApp->OnReceiveNDIImage(pixelBuffer, Width, Height, Format);
		}

		return true;
	}
}
#endif // USE_NETWORK