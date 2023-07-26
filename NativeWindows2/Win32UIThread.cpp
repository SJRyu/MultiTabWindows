#include <pch.h>
#include <NativeWindows2/Win32UIThread.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/cctrl/CDialog.h>

Win32UIThread::Win32UIThread()
{
	res_ = std::unique_ptr<IndependentRes>(new IndependentRes());

	evStarted_ = CreateEventW(
		nullptr,
		true, // manual reset
		false, // initial set
		nullptr
	);
	assert(evStarted_ != nullptr);
}

Win32UIThread::~Win32UIThread()
{
	Close();
	SafeClose(&evStarted_);
}

void Win32UIThread::Start()
{
	hthread_ = CreateThread(
		nullptr,
		0,
		threadroutine,
		this,
		0,
		&threadid_
	);
#ifdef _DEBUG
	assert(WaitForSingleObject(evStarted_, INFINITE) == WAIT_OBJECT_0);
#else
	assert(WaitForSingleObject(evStarted_, 5000) == WAIT_OBJECT_0);
#endif
}

void Win32UIThread::Start1()
{
	hthread_ = CreateThread(
		nullptr,
		0,
		threadroutine,
		this,
		0,
		&threadid_
	);

	DWORD ret;
	while (true)
	{
		ret = MsgWaitForMultipleObjects(
			1, &evStarted_, FALSE, INFINITE, QS_SENDMESSAGE);
		if (ret == WAIT_OBJECT_0)
		{
			break;
		}
		else if (ret == WAIT_OBJECT_0 + 1)
		{
			MSG Msg;
			if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE))
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

void Win32UIThread::Close()
{
	if (evStarted_)
	{
		if (WaitForSingleObject(evStarted_, 0) == WAIT_OBJECT_0)
		{
			BOOL ret = PostThreadMessage(threadid_, UTM_QUIT, 0, 0);
			if (ret == FALSE)
			{
				auto err = GetLastError();
				assert(0);
			}
			assert(WaitForSingleObject(hthread_, INFINITE) != WAIT_TIMEOUT);

			SafeClose(&hthread_);
		}
	}
	else
	{
		assert(0);
	}
}

DWORD Win32UIThread::threadroutine(void* instance)
{
	return ((Win32UIThread*)instance)->threadroutine();
}

DWORD Win32UIThread::threadroutine()
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
#if 0
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
#endif
	hmsgw_ = ::CreateWindowEx(
		0, 
		msgwclass_.lpszClassName, 
		0, 0, 0, 0, 0, 0, 
		HWND_MESSAGE, 
		0, g_hinst, 0);

	OnThreadStarts(this);
	SetEvent(evStarted_);

	MSG Msg;
	while (GetMessage(&Msg, nullptr, 0, 0))
	{
		switch (Msg.message)
		{
		case UTM_QUIT:
		{
			::DestroyWindow(hmsgw_);
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE));
			::PostQuitMessage(0);
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
#if 0
	cts.cancel();
	vbitask.wait();
#endif
	res_->ReleaseIndependentRes();

	OnThreadEnds(this);
	ResetEvent(evStarted_);
	return 0;
}

LRESULT Win32UIThread::ThreadMsgProc(
	HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case UM_CREATEEX:
	{
		auto win = (Win32Window*)wp;
		win->CreateEx();
		if (lp) win->ShowWindow();
		return 0;
	}
	case UM_CLOSE:
	{
		auto win = (Win32Window*)wp;
		if (win->hwnd_)
		{
			::SendMessage(win->hwnd_, UM_CLOSE, 0, 0);
			SetEvent(win->evclosed_);
		}
		return 0;
	}
	case UM_CREATEDLG:
	{
		auto dlg = (CDialog*)wp;
		dlg->CreateCDlg();
		if (lp) dlg->ShowWindow();
		return 0;
	}
	case UM_CLOSEDLG:
	{
		auto dlg = (CDialog*)wp;
		if (dlg->hwnd_)
		{
			::SendMessage(dlg->hwnd_, UM_CLOSEDLG, 0, 0);
			SetEvent(dlg->evclosed_);
		}
		return 0;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
