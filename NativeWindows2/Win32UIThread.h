#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <NativeWindows2/windows/WindowProperties.h>
#include <NativeWindows2/directx/IndependentRes.h>

namespace NativeWindows
{
	class Win32Window;

	class NATIVEWINDOWS2_API Win32UIThread
	{
	public:

		HANDLE hthread_ = nullptr;
		DWORD threadid_ = 0;

		HANDLE evStarted_ = nullptr;

		HWND hmsgw_ = nullptr;

		std::unique_ptr<IndependentRes> res_;

		Win32UIThread();
		virtual ~Win32UIThread();
		Win32UIThread(const Win32UIThread& r) = delete;
		Win32UIThread& operator=(const Win32UIThread& r) = delete;

		virtual void Start();
		virtual void Start1();
		virtual void Close();

		std::function<void(Win32UIThread*)> OnThreadStarts = [](Win32UIThread*) {};
		std::function<void(Win32UIThread*)> OnThreadEnds = [](Win32UIThread*) {};

		static inline WNDCLASSEX msgwclass_;
		static inline void RegisterMsgWindow()
		{
			msgwclass_.cbSize = sizeof(WNDCLASSEX);
			msgwclass_.hInstance = g_hinst;
			msgwclass_.lpfnWndProc = ThreadMsgProc;
			msgwclass_.lpszClassName = L"msgonly";
			ATOM ret = RegisterClassEx(&msgwclass_);
			assert(ret != NULL);
		}

		static inline void UnregisterMsgWindow()
		{
			UnregisterClass(msgwclass_.lpszClassName, g_hinst);
		}

		static LRESULT CALLBACK ThreadMsgProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	protected:

		static DWORD WINAPI threadroutine(void* instance);
		virtual DWORD threadroutine();
	};
}