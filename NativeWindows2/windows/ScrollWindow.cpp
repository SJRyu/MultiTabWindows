#include <pch.h>
#include <NativeWindows2/windows/ScrollWindow.h>
#include <NativeWindows2/MainLoop.h>

ScrollNotch::ScrollNotch(D2dWinArgs const& args) :
	D2dWindow1(args)
{

}

ScrollNotch::~ScrollNotch()
{

}

LRESULT ScrollNotch::OnCreate1(LPCREATESTRUCT createstr)
{
	visualbg_ = AddColorVisual(Windows::UI::Colors::AliceBlue());
	return 0;
}

LRESULT ScrollNotch::OnLbtndown(int state, int x, int y)
{
	return 0;
}

/*********************************************************************/

void ScrollWindow::OnClose1()
{
	hscroll_.reset();
	vscroll_.reset();
	notch_.reset();
	target_.reset();
}

LRESULT ScrollWindow::OnCreate1(LPCREATESTRUCT createstr)
{
	auto w = rect_.width;
	auto h = rect_.height;
	clientRect_ = { 0, 0, w, h };

	visualbg_ = AddColorVisual(bgcolor_);

	auto thick = DpiVal(SCROLLBARTHICK);
	RECT rc{ 0, 0, thick, thick };
	notch_ = wmake_unique<ScrollNotch>(D2dWinArgs{ &rc, this });
	notch_->CreateEx();
	notch_->SetWindowPos(HWND_TOP, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW1);

	rc = { 0, thick, thick, h };
	vscroll_ = wmake_unique<Scrollbar>(D2dWinArgs{ &rc, this });
	vscroll_->CreateEx();
	vscroll_->SetWindowPos(HWND_TOP, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW1);

	rc = { thick, 0, w, thick };
	hscroll_ = wmake_unique<Scrollbar>(D2dWinArgs{ &rc, this }, true);
	hscroll_->CreateEx();
	hscroll_->SetWindowPos(HWND_TOP, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW1);

	target_->CreateEx();

	notch_->PlaceAtTop();
	vscroll_->PlaceAtTop();
	hscroll_->PlaceAtTop();

	return 0;
}

LRESULT ScrollWindow::OnSize(WPARAM state, int width, int height)
{
	clientRect_.right = width;
	clientRect_.bottom = height;

	auto thick = DpiVal(SCROLLBARTHICK);
	vscroll_->SetWindowPos(0,
		0, thick, thick, height - thick,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	hscroll_->SetWindowPos(0,
		thick, 0, width - thick, thick,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	UpdateScroll();
	UpdateScrollbar();
	return 0;
}

LRESULT ScrollWindow::OnDpichangedAfterparent()
{
	auto thick = DpiVal(SCROLLBARTHICK);

	notch_->SetWindowPos(0,
		0, 0, thick, thick,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	int dx = 0, dy = 0;

	if (isVScrolling_)
	{
		dx = thick - clientRect_.left;
		clientRect_.left = thick;
	}
	if (isHScrolling_)
	{
		dy = thick - clientRect_.top;
		clientRect_.top = thick;
	}

	if (dx || dy)
	{
		ScrollTarget(targetRect_.left + dx, targetRect_.top + dy);
	}

	return 0;
}

BOOL ScrollWindow::OnMouseWheel(WPARAM wp, LPARAM lp)
{
	//WHEEL_DELTA
	auto delta = GET_WHEEL_DELTA_WPARAM(wp);
	auto key = GET_KEYSTATE_WPARAM(wp);

	if (key == MK_SHIFT)
	{
		if (delta > 0)
		{
			HScrollup();
		}
		else
		{
			HScrolldown();
		}
	}
	else
	{
		if (delta > 0)
		{
			VScrollup();
		}
		else
		{
			VScrolldown();
		}
	}

	return TRUE;
}

LRESULT ScrollWindow::UserMsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case UM_SCROLLTARGET:
	{
		target_->MoveWindow((int)wParam, (int)lParam);
		break;
	}
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void ScrollWindow::ScrollTarget(int x, int y)
{
	//::PostMessage(hwnd_, UM_SCROLLTARGET, (int)x, (int)y);
	target_->MoveWindow(x, y); //must be synchronized
	UpdateScrollbar();
}

void ScrollWindow::UpdateScrollbar()
{
	targetRect_ = target_->rect_;
	ScrollInfo info;

	if (isHScrolling_)
	{
		info.pageSize_ = targetRect_.width;
		info.clientSize_ = clientRect_.width;
		info.clientLoc_ = clientRect_.left - targetRect_.left;

		hscroll_->UpdateScrollbar(info);
	}

	if (isVScrolling_)
	{
		info.pageSize_ = targetRect_.height;
		info.clientSize_ = clientRect_.height;
		info.clientLoc_ = clientRect_.top - targetRect_.top;

		vscroll_->UpdateScrollbar(info);
	}
}

void ScrollWindow::UpdateScroll()
{
	targetRect_ = target_->rect_;
	//update scrollbar
	auto tw = targetRect_.width;
	auto th = targetRect_.height;

	auto w = rect_.width;
	auto h = rect_.height;
	auto thick = DpiVal(SCROLLBARTHICK);

	POINT pt{ targetRect_.left, targetRect_.top };
	LONG dy = 0, dx = 0;

	if (th > h)
	{
		clientRect_.left = thick;
	}
	else
	{
		clientRect_.left = 0;
	}

	if (tw > clientRect_.width)
	{
		if (isHScrolling_ == false)
		{
			dy = thick;
			clientRect_.top = thick;
			isHScrolling_ = true;
			hscroll_->ShowWindow();
		}
	}
	else
	{
		if (isHScrolling_)
		{
			dy = -thick;
			clientRect_.top = 0;
			isHScrolling_ = false;
			hscroll_->ShowWindow(SW_HIDE);
		}
	}

	if (th > clientRect_.height)
	{
		if (isVScrolling_ == false)
		{
			dx = thick;
			clientRect_.left = thick;
			isVScrolling_ = true;
			vscroll_->ShowWindow();
		}
	}
	else
	{
		if (isVScrolling_)
		{
			dx = -thick;
			clientRect_.left = 0;
			isVScrolling_ = false;
			vscroll_->ShowWindow(SW_HIDE);
		}
	}

	if (isVScrolling_)
	{
		if (targetRect_.bottom + dy < clientRect_.bottom)
		{
			dy = clientRect_.bottom - targetRect_.bottom;
		}
	}
	else
	{
		dy = clientRect_.top - targetRect_.top;
	}

	if (isHScrolling_)
	{
		if (targetRect_.right + dx < clientRect_.right)
		{
			dx = clientRect_.right - targetRect_.right;
		}
	}
	else
	{
		dx = clientRect_.left - targetRect_.left;
	}

	if (dx || dy)
	{
		pt.x += dx;
		pt.y += dy;
		ScrollTarget(pt.x, pt.y);
	}

	if (isVScrolling_ || isHScrolling_)
	{
		notch_->ShowWindow(SW_SHOW);
	}
	else
	{
		notch_->ShowWindow(SW_HIDE);
	}
}

void ScrollWindow::OnScrollChanged(int newcl, bool bHorizontal)
{
	int pageloc;
	if (bHorizontal)
	{
		pageloc = clientRect_.left - newcl;
		ScrollTarget(pageloc, targetRect_.top);
	}
	else
	{
		pageloc = clientRect_.top - newcl;
		ScrollTarget(targetRect_.left, pageloc);
	}
}

bool ScrollWindow::VScrollup()
{
	if (isVScrolling_ == false) return false;

	auto dy = DpiVal(SCROLLUNITPIXEL);

	if (isHScrolling_)
	{
		dy = std::min(dy, DpiVal(SCROLLBARTHICK) - targetRect_.top);
	}
	else
	{
		dy = std::min(dy, -targetRect_.top);
	}
	ScrollTarget(targetRect_.left, targetRect_.top + dy);

	return true;
}

bool ScrollWindow::VScrolldown()
{
	if (isVScrolling_ == false) return false;

	auto h = rect_.height;

	auto dy = -DpiVal(SCROLLUNITPIXEL);
	auto rest = h - targetRect_.bottom;
	dy = std::max(dy, rest);

	ScrollTarget(targetRect_.left, targetRect_.top + dy);

	return true;
}

bool ScrollWindow::HScrollup()
{
	if (isHScrolling_ == false) return false;

	auto dx = DpiVal(SCROLLUNITPIXEL);
	if (isVScrolling_)
	{
		dx = std::min(dx, DpiVal(SCROLLBARTHICK) - targetRect_.left);
	}
	else
	{
		dx = std::min(dx, -targetRect_.left);
	}
	ScrollTarget(targetRect_.left + dx, targetRect_.top);

	return true;
}

bool ScrollWindow::HScrolldown()
{
	if (isHScrolling_ == false) return false;

	auto w = rect_.width;

	auto dx = -DpiVal(SCROLLUNITPIXEL);
	auto rest = w - targetRect_.right;
	dx = std::max(dx, rest);

	ScrollTarget(targetRect_.left + dx, targetRect_.top);

	return true;
}

LRESULT ClientScroll::OnSize(WPARAM state, int width, int height)
{
	clientRect_.right = width;
	clientRect_.bottom = height;

	auto thick = DpiVal(SCROLLBARTHICK);
	vscroll_->SetWindowPos(0,
		0, thick, thick, height - thick,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	hscroll_->SetWindowPos(0,
		thick, 0, width - thick, thick,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	int tw, th;
	int minw = DpiVal(minw_);
	int minh = DpiVal(minh_);

	targetRect_ = target_->rect_;
	if (width >= minw) //hscroll off
	{
		th = height;
	}
	else //hscroll on
	{
		th = height - thick;
	}

	if (th >= minh) //vscroll off
	{
		tw = width;
	}
	else //vscroll on
	{
		tw = width - thick;
	}

	if (tw >= minw) //hscroll off
	{
		th = height;
	}
	else //hscroll on
	{
		th = height - thick;
	}

	target_->SetWindowPos(0,
		0, 0, tw, th,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);

	UpdateScroll();
	UpdateScrollbar();

	return 0;
}