#include <pch.h>
#include <dcomp.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/Win32UIThread.h>
#include <NativeWindows2/MainLoop.h>

void Win32Window::SetWindowArgs(WinArgs const& args)
{
	if (args.rect != nullptr)
	{
		rect_ = *args.rect;
	}
	parent_ = args.parent;

	if (parent_ == nullptr)
	{
		isRoot_ = true;
		root_ = this;
		thread_ = args.thread;

		ncm_ = new NCMeasurement{};
	}
	else
	{
		isRoot_ = false;
		wstyle_ |= WS_CHILD;
		wstylex_ |= WS_EX_NOPARENTNOTIFY;

		if (args.thread == nullptr)
		{
			thread_ = parent_->thread_;
		}
		else
		{
			thread_ = args.thread;
		}

		root_ = parent_->root_;
		ncm_ = root_->ncm_;
	}
}

Win32Window::Win32Window()
{
	evclosed_ = CreateEvent(NULL, TRUE, TRUE, NULL);
}

Win32Window::Win32Window(WinArgs const& args)
{
	evclosed_ = CreateEvent(NULL, TRUE, TRUE, NULL);
	SetWindowArgs(args);
}

Win32Window::~Win32Window()
{
	CloseHandle(evclosed_);

	if (isRoot_)
	{
		SafeDelete(&ncm_);
	}
}

HWND Win32Window::SetParent(Win32Window* parent)
{
	auto oldparent = parent_;
	parent_ = parent;
	root_ = parent->root_;
	ncm_ = root_->ncm_;

	auto ret = ::SetParent(hwnd_, parent->hwnd_);
	if (ret == NULL)
	{
		parent_ = oldparent;
		root_ = parent->root_;
		ncm_ = root_->ncm_;
		dbg_msg("error on SetParent (%x)", GetLastError());
	}
	else
	{
		::EnumChildWindows(hwnd_, EnumChildOnRootChanged, (LPARAM)root_);
	}
	return ret;
}

void Win32Window::CreateEx1()
{
	assert(::SendMessageTimeout(
		thread_->hmsgw_, UM_CREATEEX, (WPARAM)this, 0,
		SMTO_NOTIMEOUTIFNOTHUNG, 5000, NULL) != 0);
}

void Win32Window::WaitClose()
{
	auto threadid = GetCurrentThreadId();

	if (threadid == thread_->threadid_)
	{
		if (hwnd_)
		{
			::SendMessage(hwnd_, UM_CLOSE, 0, 0);
			SetEvent(evclosed_);
		}
	}
	else
	{
		::PostMessage(thread_->hmsgw_, UM_CLOSE, (WPARAM)this, 0);
		DWORD ret;
		while (true)
		{
			/**
			* RSJ: 같은 thread의 sendmessage는 peekmessage를 거치지 않고 바로 처리된다.
			* 다만 QS_SENDMESSAGE는 지정되어야 한다.
			**/
			ret = MsgWaitForMultipleObjects(
				1, &evclosed_, FALSE, INFINITE, QS_SENDMESSAGE | QS_POSTMESSAGE);
			if (ret == WAIT_OBJECT_0 || ret == WAIT_ABANDONED_0)
			{
				break;
			}
			else if (ret == WAIT_OBJECT_0 + 1)
			{
				MSG Msg;
				if (PeekMessage(&Msg, nullptr, UM_CLOSE, UM_CLOSEDLG, PM_REMOVE))
				{
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				}
			}
			else
			{
				assert(0);
			}
		}
	}
}

void Win32Window::AdjustNcSize(UINT dpi, NCMeasurement* out)
{
	int cxframe = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
	int cyframe = GetSystemMetricsForDpi(SM_CYFRAME, dpi);

	int padded = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
	int hframe = cxframe + padded;
	int vframe = cyframe + padded;

	int captionh = GetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	captionh += hframe;
	captionh += hframe;

	out->hborder = cxframe;
	out->vborder = cyframe;
	out->padded = padded;

	out->hframe = hframe;
	out->vframe = vframe;
	out->captionh = captionh;

	out->sysbtnw = GetSystemMetricsForDpi(SM_CXSIZE, dpi);
	out->sysbtnh = GetSystemMetricsForDpi(SM_CYSIZE, dpi);
	out->sysmenuw = (out->sysbtnw + cxframe) * 3;

	out->taby = hframe;
	out->tabh = captionh - hframe;

	out->d1e0 = Dpiadj(1.0f, dpi);
	out->d2e0 = Dpiadj(2.0f, dpi);
}

LRESULT Win32Window::EnterProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* create = (CREATESTRUCT*)lParam;
		Win32Window* instance = (Win32Window*)(create->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)instance);

		instance->OnNcCreate_(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Window::OnNcCreate_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (isRoot_ == false)
	{
		oldproc_ = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)PreProc_);
	}
	else
	{
		UINT dpi = ::GetDpiForWindow(hWnd); 
		ncm_->dpix = dpi;  ncm_->dpiy = dpi;

		AdjustNcSize(dpi, ncm_);

		auto sc = thread_->res_->TestSwapchainForHwnd(hWnd);
		sc->Release();

		oldproc_ = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)PreProcForRoot_);
	}
	return TRUE;
}

LRESULT Win32Window::PreProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win32Window* instance = (Win32Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return instance->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Window::PreProcForRoot_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win32Window* instance = (Win32Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	{
		LRESULT hr;
		if (DwmDefWindowProc(hWnd, uMsg, wParam, lParam, &hr))
		{
			return hr;
		}
	}
	return instance->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Win32Window::OnMouseEvent(UINT msg, WPARAM wp, LPARAM lp)
{
	// need to call SetFocus somewhere
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		return OnMousemove((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_MOUSEWHEEL:
	{
		if (OnMouseWheel(wp, lp))
		{
			return 0;
		}
		else
		{
			//transparenting
			break;
		}
	}
	case WM_LBUTTONDOWN:
	{
		::SetFocus(hwnd_);
		return OnLbtndown((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_LBUTTONUP:
	{
		return OnLbtnup((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_LBUTTONDBLCLK:
	{
		return OnLbtndouble((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_RBUTTONDOWN:
	{
		::SetFocus(hwnd_);
		return OnRbtndown((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_RBUTTONUP:
	{
		return OnRbtnup((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_RBUTTONDBLCLK:
	{
		return OnRbtndouble((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_MBUTTONDOWN:
	{
		::SetFocus(hwnd_);
		return OnMbtndown((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_MBUTTONUP:
	{
		return OnMbtnup((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_MBUTTONDBLCLK:
	{
		return OnMbtndouble((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_XBUTTONDOWN:
	{
		return OnXbtndown((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_XBUTTONUP:
	{
		return OnXbtnup((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	case WM_XBUTTONDBLCLK:
	{
		return OnXbtndouble((int)wp, GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	}
	default:
		break;
	}
	return DefWindowProc(hwnd_, msg, wp, lp);
}

LRESULT Win32Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	{
		//dbg_msg("mouseEv = %x", uMsg);
		return OnMouseEvent(uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_NCHITTEST:
	{
		return OnNcHitTest(lParam);
	}
	case WM_SIZING:
	{
		OnSizing(wParam, lParam);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	case WM_SIZE:
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		rect_.width = width;
		rect_.height = height;
		return OnSize(wParam, width, height);
	}
	case WM_MOVE:
	{
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		rect_.MoveToXY(x, y);
		return OnMove(x, y);
	}
	case WM_ENTERSIZEMOVE:
	{
		isSizing_ = true;
		return OnEnterSizemove();
	}
	case WM_EXITSIZEMOVE:
	{
		isSizing_ = false;
		return OnExitSizemove();
	}	
	case WM_MOUSELEAVE:
	{
		return OnMouseleave();
	}
	case WM_MOUSEHOVER:
	{
		return OnMouseHover(wParam, lParam);
	}
	case WM_PAINT:
	{
		return OnPaint();
	}
	case WM_ERASEBKGND:
	{
		return true;
	}
	case WM_CONTEXTMENU:
	{
		if (OnContextMenu((HWND)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
		{
			return 0;
		}
		else
		{
			break;
		}
	}
	case WM_COMMAND:
	{
		if (OnCommand(wParam, lParam))
		{
			return 0;
		}
		else
		{
			break;
		}
	}
	case UM_BTNCLICKED:
	{
		return OnCButtonClicked(wParam, lParam);
	}
	case UM_DPICHANGED_AFTERPARENT:
	{
		return OnDpichangedAfterparent();
	}
	case WM_DPICHANGED_AFTERPARENT:
	{
		PostMessage(hwnd_, UM_DPICHANGED_AFTERPARENT, 0, 0);
		break;
	}
	case WM_CREATE:
	{
		hwnd_ = hWnd;
		auto ret = OnCreate((LPCREATESTRUCT)lParam);
		ResetEvent(evclosed_);
		return ret;
	}
	case UM_CLIENT_RESIZE:
	{
		OnClientResize(wParam, lParam);
		return 0;
	}
	case UM_ROOT_CHANGED:
	{
		root_ = (Win32Window*)lParam;
		ncm_ = root_->ncm_;
		return 0;
	}
	case WM_CLOSE:
	{
		PostMessage(hwnd_, UM_CLOSE, 0, 0);
		return 0;
	}
	case UM_CLOSE:
	{
		OnClose();
		DestroyWindow(hwnd_);
		return 0;
	}
	case WM_DESTROY:
	{
		OnDestroy();
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)oldproc_);
		hwnd_ = nullptr;
		return 0;
	}
	case WM_DPICHANGED:
	{
		//only for root window
		ncm_->dpix = LOWORD(wParam);
		ncm_->dpiy = HIWORD(wParam);
		AdjustNcSize(ncm_->dpiy, ncm_);

		OnDpichanged(uMsg, wParam, lParam);
		return 0;
	}
	}
	return UserMsgHandler(hWnd, uMsg, wParam, lParam);
}
