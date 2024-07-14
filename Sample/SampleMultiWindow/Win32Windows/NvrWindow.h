#pragma once

#include <NativeWindows2/windows/D2dWindow.h>
#include <Win32Windows/VideoView.h>
#include <Win32Windows/TestWindow1.h>

namespace NativeWindows
{
	class NvrTab;
	class VideoView;
	class TestWindow1;

	class VideoPannel : public D2dWindow1
	{
	public:

		inline VideoPannel(NvrTab* tab) : tab_(tab)
		{
			wstyle_ |= WS_VISIBLE;
		};
		virtual ~VideoPannel() {}

		NvrTab* tab_;

		std::array<wunique_ptr<VideoView>, MAX_CAM_COUNT> views_;
		wunique_ptr<TestWindow1> svgwin_;

		static constexpr int MIN_WIDTH = 1280;
		static constexpr int MIN_HEIGHT = 720;

		virtual void WINAPI GetMinSize(int* w, int* h) override
		{ 
			*w = MIN_WIDTH;
			*h = MIN_HEIGHT;
		}

		virtual void CALLBACK OnPageResize(int w, int h) override
		{
			SetWindowPos(0, 0, 0,
				w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);
		}

		virtual void CreateEx() override
		{
			rect_.width = std::max((int)rect_.width, DpiVal(MIN_WIDTH));
			rect_.height = std::max((int)rect_.height, DpiVal(MIN_HEIGHT));
			D2dWindow1::CreateEx();
		}

		virtual void WINAPI SetWindowPos(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags) override
		{
			if ((flags & SWP_NOSIZE) == 0)
			{
				cx = std::max(cx, DpiVal(MIN_WIDTH));
				cy = std::max(cy, DpiVal(MIN_HEIGHT));
			}
			D2dWindow1::SetWindowPos(insertafter, x, y, cx, cy, flags);
		}

	protected:

		void WINAPI CalcChildRect(int pos, CRECT<LONG>* outref);

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual VOID CALLBACK OnClose1() override;
	};
}
