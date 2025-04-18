#include <pch.h>
#include <Win32Windows/NvrWindow.h>
#include <Win32Windows/NvrTab.h>
#include <NativeWindows2/windows/ClientWindow.h>

using namespace NativeWindows;

VOID VideoPannel::OnClose1()
{
	svgwin_.reset();
	std::fill(views_.begin(), views_.end(), nullptr);
}

LRESULT VideoPannel::OnCreate1(LPCREATESTRUCT createstr)
{
	int x, y;
	int cw = rect_.width / CAMS_IN_ROW;
	int ch = rect_.height / CAMS_IN_ROW;
	for (int pos = 0; pos < MAX_CAM_COUNT; pos++)
	{
		x = (pos % CAMS_IN_ROW) * cw;
		y = (pos / CAMS_IN_ROW) * ch;

		RECT rect{ x + 1, y + 1, x + cw - 2, y + ch - 2 };

		views_[pos] = wmake_unique<VideoView>(
			WinArgs{ &rect, this }, pos, pos);
		views_[pos]->CreateEx();
	}

	x = rect_.width / 2;
	y = rect_.height / 2;
	int cx = DpiVal(120), cy = DpiVal(120);
	x -= cx / 2;
	y -= cy / 2;
	RECT rect{ x, y, x + cx, y + cy };
	svgwin_ = wmake_unique<TestWindow1>(WinArgs{ &rect, this });
	svgwin_->CreateEx();
	svgwin_->ShowWindow();

	return 0;
}

LRESULT VideoPannel::OnSize(WPARAM state, int width, int height)
{
	int x, y;
	int cw = width / CAMS_IN_ROW;
	int ch = height / CAMS_IN_ROW;
	for (int pos = 0; pos < MAX_CAM_COUNT; pos++)
	{
		x = (pos % CAMS_IN_ROW) * cw;
		y = (pos / CAMS_IN_ROW) * ch;

		views_[pos]->SetWindowPos(0,
			x + 1, y + 1, cw - 2, ch - 2,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);
	}

	x = rect_.width / 2;
	y = rect_.height / 2;
	int cx = DpiVal(120), cy = DpiVal(120);
	x -= cx / 2;
	y -= cy / 2;
	svgwin_->SetWindowPos(HWND_TOP,
		x, y, cx, cy,
		SWP_NOACTIVATE | SWP_NOREDRAW1);
	return 0;
}

void VideoPannel::CalcChildRect(int pos, CRECT<LONG>* outref)
{
	int cw = rect_.right / CAMS_IN_ROW;
	int ch = rect_.height / CAMS_IN_ROW;

	int x = (pos % CAMS_IN_ROW) * cw;
	int y = (pos / CAMS_IN_ROW) * ch;

	outref->left = x;
	outref->top = y;
	outref->width = cw;
	outref->height = ch;
}
