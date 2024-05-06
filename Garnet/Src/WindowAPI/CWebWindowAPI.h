#pragma once
#ifdef USE_WEB_NATIVE
#include <string>
#include <memory>
#include "../Interface/IWindowAPI.h"

namespace app { class CAppCore; }

namespace window
{
	class CWebWindowAPI : public IWindowAPI
	{
		app::CAppCore* m_pCAppCore;

		int m_Width;
		int m_Height;

	private:
		void KeyAction(std::string key, bool IsDown);
	public:
		CWebWindowAPI();
		virtual ~CWebWindowAPI() = default;

		virtual bool Release() override;

		virtual bool Initialize(app::CAppCore* pAppCore, int Width, int Height) override;

		virtual void SwapWindowBuffers() override;

		virtual void AssignCurrentWindowSize() override;

		virtual void PollEvents() override;

		virtual app::CAppCore* GetAppCore() const override;

		virtual void ResizeWindow(int w, int h) override;

		// インプットイベント
		virtual void OnKeyDown(std::string key) override;
		virtual void OnKeyUp(std::string key) override;

		// リサイズイベント
		virtual void OnResize(int w, int h) override;

		// フォーカスイベント
		virtual void OnFocus(int focused) override;

		// マウスイベント
		virtual void OnMouseDown(int buttonNum, int x, int y) override;
		virtual void OnMouseUp(int buttonNum, int x, int y) override;
		virtual void OnMouseMove(int x, int y) override;
		virtual void OnMouseWheel(int deltaY) override;
	};
}
#endif // USE_WEB_NATIVE