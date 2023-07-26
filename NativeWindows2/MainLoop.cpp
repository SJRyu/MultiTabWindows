#include <pch.h>
#include <MainLoop.h>
#include <NativeWindows2/windows/WindowProperties.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/windows/ContainerWindow.h>
#include <NativeWindows2/windows/Ctab.h>
#include <NativeWindows2/windows/TabWindow.h>
#include <NativeWindows2/windows/CButton.h>

using namespace NativeWindows;
using namespace winrt;

MainLoop::MainLoop()
{
}

MainLoop::~MainLoop()
{
}

static inline long long milliseconds_now() {
	LARGE_INTEGER s_frequency;
	QueryPerformanceFrequency(&s_frequency);
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (1000LL * now.QuadPart) / s_frequency.QuadPart;
}

void MainLoop::Start()
{
	winrt::init_apartment(apartment_type::single_threaded);
	winrt::Windows::System::DispatcherQueueController controller{ nullptr };
	DispatcherQueueOptions qopt = {
		sizeof(DispatcherQueueOptions),
		DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE };
	check_hresult(CreateDispatcherQueueController(
		qopt, reinterpret_cast<
		ABI::Windows::System::IDispatcherQueueController**>(put_abi(controller))));

	res_->InitIndependentRes();

	IsGUIThread(TRUE);
	threadid_ = GetCurrentThreadId();
	hthread_ = GetCurrentThread();

	concurrency::cancellation_token_source cts;
	auto ct = cts.get_token();
	auto vbitask = concurrency::create_task([&]
		{
			while (true)
			{
				if (ct.is_canceled()) break;
				DwmFlush();
			}
		}, ct);

	hmsgw_ = ::CreateWindowEx(
		0,
		msgwclass_.lpszClassName,
		0, 0, 0, 0, 0, 0,
		HWND_MESSAGE,
		0, g_hinst, 0);

	SetEvent(evStarted_);

	OnAppStart();

	MSG Msg;
	while (GetMessage(&Msg, nullptr, 0, 0))
	{
		switch (Msg.message)
		{
		case UTM_CLOSECONTAINER:
		{
			CloseContainer(Msg.wParam);
			break;
		}
		default:
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
			break;
		}
		}
	}

	cts.cancel();
	vbitask.wait();

	OnAppEnd();

	res_->ReleaseIndependentRes();
	ResetEvent(evStarted_);
}

ContainerWindow* MainLoop::NewContainer(RECT& rect)
{
	int idx = (int)windows_.size();
	auto win = new ContainerWindow(&rect, this, idx);

	win->CreateEx();
	windows_.push_back(wunique_ptr<ContainerWindow>(win));

	OnNewContainer(win);

	return win;
}

ContainerWindow* MainLoop::FindDock(int x, int y)
{
	ContainerWindow* ret = nullptr;
	POINT pt{ x, y };
	HWND hwnd = WindowFromPoint(pt);
	if (hwnd)
	{
		auto it = std::find_if(windows_.begin(), windows_.end(),
			[hwnd](wunique_ptr<ContainerWindow>& el)
			{
				return (el->hwnd_ == hwnd);
			}
		);

		if (it != windows_.end())
		{
			auto win = it->get();
			if (win->ctab_->tabs_.size() < win->tabmaxn_)
			{
				RECT rc = win->rect_;
				rc.right -= win->ncm_->sysmenuw;
				rc.right -= win->ncm_->sysmenuw;
				rc.bottom = rc.top + win->ncm_->captionh - win->ncm_->hframe - win->ncm_->hframe;

				if (::PtInRect(&rc, pt))
				{
					ret = win;
				}
			}
		}
	}

	return ret;
}

void MainLoop::CloseContainer(uintptr_t instance)
{
	for (auto n = windows_.begin(); n != windows_.end(); n++)
	{
		if ((uintptr_t)n->get() == instance)
		{
			windows_.erase(n);
			break;
		}
	}

	if (windows_.size() == 0)
	{
		::DestroyWindow(hmsgw_);
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE));
		::PostQuitMessage(0);
	}
}
