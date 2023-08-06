#pragma once

#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/directx/D2dComponents.h>
#include <NativeWindows2/windows/WindowProperties.h>

namespace NativeWindows
{
	class Win32UIThread;

	class NATIVEWINDOWS2_API D2dWindow : public Win32Window, public D2dComponents
	{
	public:
		inline D2dWindow() : D2dComponents(this) {}

		inline D2dWindow(WinArgs const& args) :
			Win32Window(args), D2dComponents(this) {}

		virtual ~D2dWindow() 
		{
		}

	protected:

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) { return 0; };
		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override
		{
			InitCompositor();
			return OnCreate1(createstr);
		}

		virtual void CALLBACK OnClose1() {};
		virtual void CALLBACK OnClose() override
		{
			OnClose1();
#ifdef WINRT_CLOSETEST
			ReleaseCompositor();
#endif
		}
	};

	struct D2dWinArgs
	{
		RECT* rect = nullptr;
		D2dWindow* parent = nullptr;
		Win32UIThread* thread = nullptr;
	};

	class NATIVEWINDOWS2_API D2dWindow1 : public D2dWindow
	{
	public:

		inline D2dWindow1() {}
		inline D2dWindow1(D2dWinArgs const& args)
		{
			SetD2dArgs(args);
		}

		virtual ~D2dWindow1()
		{
			if (winrt::get_abi(parentv_))
			{
				parentv_.Remove(rootv_);
			}
		}

		inline void SetD2dArgs(D2dWinArgs const& args)
		{
			SetWindowArgs({ args.rect, args.parent, args.thread });
			d2dparent_ = args.parent;
		}

		D2dWindow* d2dparent_;

		virtual HWND WINAPI SetParent(Win32Window* parent) override
		{
			auto ret = Win32Window::SetParent(parent);
			if (ret != NULL)
			{
				d2dparent_ = static_cast<D2dWindow*>(parent);

				NewCompParent(d2dparent_);
			}
			return ret;
		}

		virtual void WINAPI ShowWindow(int nCmdShow = SW_SHOW) override
		{
			if (nCmdShow == SW_HIDE)
			{
				rootv_.IsVisible(false);
			}
			else
			{
				rootv_.IsVisible(true);
			}
			Win32Window::ShowWindow(nCmdShow);
		}

		virtual void WINAPI MoveWindow(int x, int y) override
		{
			MoveVisuals(x, y);
			Win32Window::MoveWindow(x, y);
		}

		virtual void WINAPI SetWindowPos(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags) override
		{
			if (!(flags & SWP_NOMOVE)) MoveVisuals(x, y);
			if (!(flags & SWP_NOSIZE)) SizeVisuals(cx, cy);
			Win32Window::SetWindowPos(insertafter, x, y, cx, cy, flags);
		}

		virtual void WINAPI AnimeMoveWindow(int x, int y)
		{
			AnimeMoveVisuals(x, y);
			Win32Window::MoveWindow(x, y);
		}

		virtual void WINAPI AnimeWindowPos(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags)
		{
			if (!(flags & SWP_NOMOVE)) AnimeMoveVisuals(x, y);
			if (!(flags & SWP_NOSIZE)) AnimeSizeVisuals(cx, cy);
			Win32Window::SetWindowPos(insertafter, x, y, cx, cy, flags);
		}

		virtual void CreateEx() override
		{
			D2dWindow::CreateEx();
			//CreateEx 안에서 size와 move가 걸리는 경우 비주얼을 재조정해야 한다.
			MoveVisuals(rect_.left, rect_.top);
			SizeVisuals(rect_.width, rect_.height);
		}

	protected:

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override
		{
			InitCompositor1(d2dparent_);
			if (wstyle_ & WS_VISIBLE)
			{
				rootv_.IsVisible(true);
			}
			else
			{
				rootv_.IsVisible(false);
			}
			return OnCreate1(createstr);
		}

		virtual void CALLBACK OnClose() override
		{
			OnClose1();
#ifdef WINRT_CLOSETEST
			ReleaseCompositor1();
#endif
		}
	};

#if 0
template <typename... Args>
inline D2dWindow(D2D1_COLOR_F&& bg, Args&&... args)
	: Win32Window(std::forward<Args>(args)...),
	D2dComponents(this), bgColor_(bg) {}

template <typename... Args>
inline D2dWindow(Args&&... args)
	: Win32Window(std::forward<Args>(args)...),
	D2dComponents(this), bgColor_({ 0.f, 0.f, 0.f, 0.f }) {}
#endif
}
