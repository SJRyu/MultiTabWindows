#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <NativeWindows2/WinUIimports.h>
#include <NativeWindows2/directx/DXimports.h>
#include <NativeWindows2/windows/WindowProperties.h>

namespace NativeWindows
{
	class Win32Window;
	class Win32UIThread;

	struct WinArgs
	{
		RECT* rect = nullptr;
		Win32Window* parent = nullptr;
		Win32UIThread* thread = nullptr;
	};

	struct NCMeasurement
	{
		UINT dpix = 96;
		UINT dpiy = 96;

		LONG hborder;
		LONG vborder;
		LONG padded;
		LONG hframe;
		LONG vframe;
		LONG captionh;
		LONG sysbtnw;
		LONG sysbtnh;
		LONG sysmenuw;

		LONG tabh;
		LONG taby;

		float d1e0 = 1.0f;
		float d2e0 = 2.0f;
	};

	class NATIVEWINDOWS2_API Win32Window
	{
	public:
		Win32Window* parent_ = nullptr;
		CRECT<LONG> rect_{0,0,0,0};

		const WCHAR* wname_ = nullptr;

		Win32UIThread* thread_ = nullptr;
		HWND hwnd_ = nullptr;

		bool isRoot_ = false;
		Win32Window* root_ = nullptr;
		NCMeasurement* ncm_ = nullptr;

		DWORD wstylex_ = 0;
		DWORD wstyle_ = 0;

		HANDLE evclosed_;

		bool isSizing_ = false;

		Win32Window();
		Win32Window(WinArgs const& args);
		virtual ~Win32Window();
		Win32Window(Win32Window const& r) = delete;
		Win32Window& operator=(const Win32Window& r) = delete;

		void WINAPI SetWindowArgs(WinArgs const& args);

		//Child into child only allowed. and need samethread on parents.
		virtual HWND WINAPI SetParent(Win32Window* parent);

		virtual void WINAPI MinSize(int* w, int* h) { *w = 0; *h = 0; }
		virtual void CALLBACK OnPageResize(int w, int h) {}

		// assert for debugging, have to clear in release.
		virtual void WINAPI MoveWindow(int x, int y)
		{
			assert(::SetWindowPos(hwnd_, 0, x, y, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1) != FALSE);
		}

		virtual void WINAPI MoveWindowAsync(int x, int y)
		{
			assert(::SetWindowPos(hwnd_, 0, x, y, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1 | SWP_ASYNCWINDOWPOS) != FALSE);
		}

		virtual void WINAPI SetWindowPos(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags)
		{
			flags |= SWP_NOACTIVATE | SWP_NOREDRAW1;
			assert(::SetWindowPos(hwnd_, insertafter, x, y, cx, cy, flags) != FALSE);
		}

		virtual void WINAPI SetWindowPosAsync(
			HWND insertafter, int x, int y, int cx, int cy, UINT flags)
		{
			flags |= SWP_NOACTIVATE | SWP_NOREDRAW1 | SWP_ASYNCWINDOWPOS;
			assert(::SetWindowPos(hwnd_, insertafter, x, y, cx, cy, flags) != FALSE);
		}

		virtual void WINAPI ShowWindow(int nCmdShow = SW_SHOW)
		{
			::ShowWindow(hwnd_, nCmdShow);
		}

		static inline WNDCLASSEX windowclass_;
		static inline void RegisterWindowClass()
		{
			windowclass_.cbSize = sizeof(WNDCLASSEX);
			windowclass_.hInstance = g_hinst;
			windowclass_.lpfnWndProc = EnterProc_;
			windowclass_.lpszClassName = L"win32window";
			windowclass_.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowclass_.style = CS_DBLCLKS;

			ATOM ret = RegisterClassEx(&windowclass_);
			assert(ret != NULL);
		}

		static inline void UnregisterWindowClass()
		{
			UnregisterClass(windowclass_.lpszClassName, g_hinst);
		}

		virtual void CreateEx()
		{
#pragma warning(push)
#pragma warning(disable : 4312)

			HWND hwnd = CreateWindowExW(
				wstylex_,
				windowclass_.lpszClassName,
				wname_,
				wstyle_,
				rect_.left, rect_.top,
				rect_.width, rect_.height,
				((parent_ == nullptr) ? nullptr : parent_->hwnd_),
				nullptr,
				g_hinst,
				this);
#pragma warning (pop)
			assert(hwnd_ == hwnd);
		}

		virtual void CreateEx1();
		void WaitClose();

		template <typename T> auto DpiVal(T val)
		{
			return Dpiadj(val, ncm_->dpiy);
		}

		inline void RefreshView()
		{
			::RedrawWindow(hwnd_, 0, 0, RDW_INTERNALPAINT | RDW_NOCHILDREN);
		}

		inline void ValidateView()
		{
			::RedrawWindow(hwnd_, 0, 0, RDW_VALIDATE | RDW_NOCHILDREN);
		}

	protected:

		static void WINAPI AdjustNcSize(UINT dpi, NCMeasurement* out);

		WNDPROC oldproc_ = NULL;
		bool bVbi_ = false;

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) { return 0; }
		virtual void CALLBACK OnClose() {}
		virtual void CALLBACK OnDestroy() {}
		virtual void CALLBACK OnDpichanged(UINT uMsg, WPARAM wParam, LPARAM lParam) {}
		virtual LRESULT CALLBACK OnDpichangedAfterparent() { return 0; }

		virtual LRESULT CALLBACK OnNcHitTest(LPARAM lParam)
		{
			return DefWindowProc(hwnd_, WM_NCHITTEST, 0, lParam);
		};

		virtual LRESULT CALLBACK OnEnterSizemove() { return 0; }
		virtual LRESULT CALLBACK OnExitSizemove() { return 0; }

		virtual void CALLBACK OnSizing(WPARAM wParam, LPARAM lParam) {}
		virtual LRESULT CALLBACK OnMove(int x, int y) { return 0; }
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) { return 0; }
		virtual LRESULT CALLBACK OnPaint() 
		{ 
			ValidateView();
			return 0;
		}

		virtual LRESULT CALLBACK OnMousemove(int state, int x, int y) { return 0; }
		virtual LRESULT CALLBACK OnMouseleave() { return 0; }
		virtual LRESULT CALLBACK OnMouseHover(WPARAM wp, LPARAM lp) { return 0; };
		virtual BOOL CALLBACK OnMouseWheel(WPARAM wp, LPARAM lp) { return FALSE; };
		virtual LRESULT CALLBACK OnLbtndown(int state, int x, int y) { return 0; }
		virtual LRESULT CALLBACK OnLbtndouble(int state, int x, int y) { return 0; }
		virtual LRESULT CALLBACK OnLbtnup(int state, int x, int y) { return 0; }
		//virtual LRESULT CALLBACK OnRbtndown(int stae, int x, int y) { return 0; }
		virtual BOOL CALLBACK OnContextMenu(HWND hwnd, int xpos, int ypos) { return FALSE; }
		virtual LRESULT CALLBACK OnCommand(WPARAM wp, LPARAM lp) { return FALSE; }

		virtual LRESULT CALLBACK OnCButtonClicked(WPARAM wp, LPARAM lp) { return 0; }
		virtual LRESULT CALLBACK OnHScrollCmd(WPARAM wp, LPARAM lp) { return 0; }
		virtual LRESULT CALLBACK OnVScrollCmd(WPARAM wp, LPARAM lp) { return 0; }
		virtual LRESULT CALLBACK OnVbi(WPARAM wp, LPARAM lp) { return 0; }
		virtual void CALLBACK OnClientMove(WPARAM wp, LPARAM lp)
		{
			MoveWindow((int)wp, (int)lp);
		}
		virtual void CALLBACK OnClientResize(WPARAM wp, LPARAM lp) 
		{ 
			SetWindowPos(0, 0, 0, (int)wp, (int)lp,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW1);
		}

		virtual LRESULT CALLBACK UserMsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		static BOOL CALLBACK EnumChildOnRootChanged(HWND hwnd, LPARAM lparam)
		{
			::SendMessage(hwnd, UM_ROOT_CHANGED, 0, lparam);
			return TRUE;
		}

		static LRESULT CALLBACK EnterProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnNcCreate_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK PreProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK PreProcForRoot_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT CALLBACK WndProcForRoot(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	struct Deleter
	{
		void operator()(Win32Window* p)
		{
			p->WaitClose();
			::delete p;
		}
	};

	template <typename T>
	using wunique_ptr = std::unique_ptr <T, Deleter>;

	template <class _Ty, class... _Types, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
	_NODISCARD_SMART_PTR_ALLOC _CONSTEXPR23 wunique_ptr<_Ty> wmake_unique(_Types&&... _Args) 
	{
		return wunique_ptr<_Ty>(new _Ty(_STD forward<_Types>(_Args)...));
	}

	template <class _Ty, std::enable_if_t<std::is_array_v<_Ty>&& std::extent_v<_Ty> == 0, int> = 0>
	_NODISCARD_SMART_PTR_ALLOC _CONSTEXPR23 wunique_ptr<_Ty> wmake_unique(const size_t _Size) 
	{ 
		using _Elem = remove_extent_t<_Ty>;
		return wunique_ptr<_Ty>(new _Elem[_Size]());
	}

	template <class _Ty, class... _Types, std::enable_if_t<std::extent_v<_Ty> != 0, int> = 0>
	void wmake_unique(_Types&&...) = delete;
}
