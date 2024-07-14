#pragma once

#include <NativeWindows2/composition/Cpannel.h>

namespace NativeWindows
{
	class TabComps;

	class TestCpannel : public Cpannel
	{
	public:

		inline TestCpannel(TabComps* tab) : tab_(tab)
		{
			wstyle_ |= WS_VISIBLE;
			color_ = Windows::UI::Colors::YellowGreen();
		};
		virtual ~TestCpannel() {}

		TabComps* tab_;

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
			Cpannel::CreateEx();
		}

		virtual void WINAPI SetWindowPos(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags) override
		{
			if ((flags & SWP_NOSIZE) == 0)
			{
				cx = std::max(cx, DpiVal(MIN_WIDTH));
				cy = std::max(cy, DpiVal(MIN_HEIGHT));
			}
			Cpannel::SetWindowPos(insertafter, x, y, cx, cy, flags);
		}

	protected:

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual VOID CALLBACK OnClose() override;
	};
}
