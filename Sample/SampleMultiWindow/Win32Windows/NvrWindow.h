#pragma once

#include <NativeWindows2/windows/D2dWindow.h>
#include <NativeWindows2/windows/ScrollWindow.h>
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

		inline VideoPannel(D2dWinArgs const& args) : D2dWindow1(args) 
		{
			wstyle_ |= WS_VISIBLE;
		};
		virtual ~VideoPannel() {}

		std::array<wunique_ptr<VideoView>, MAX_CAM_COUNT> views_;
		wunique_ptr<TestWindow1> svgwin_;

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

		static constexpr int MIN_WIDTH = 1280;
		static constexpr int MIN_HEIGHT = 720;

	protected:

		void WINAPI CalcChildRect(int pos, CRECT<LONG>* outref);

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual VOID CALLBACK OnClose1() override;
	};

	class NvrWindow : public D2dWindow
	{
	public:

		NvrWindow(NvrTab* tab);
		virtual ~NvrWindow() {};

		NvrTab* tab_;

	protected:

		wunique_ptr<ClientScroll> scrollw_;

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual VOID CALLBACK OnClose1() override;

	};
}
