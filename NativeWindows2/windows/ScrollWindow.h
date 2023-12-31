#pragma once

#include <NativeWindows2/windows/D2dWindow.h>
#include <NativeWindows2/windows/Scrollbar.h>

/**
* There is a miss location of client, when DPI changed.
**/
namespace NativeWindows
{
	class MainLoop;
	class Scrollbar;
	
	class NATIVEWINDOWS2_API ScrollNotch : public D2dWindow1
	{
	public:

		ScrollNotch(D2dWinArgs const& args);
		virtual ~ScrollNotch();

	protected:

		SpriteVisual visualbg_{ nullptr };

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnLbtndown(int state, int x, int y) override;
	};

	class NATIVEWINDOWS2_API ScrollWindow : public D2dWindow1
	{
	public:

		inline ScrollWindow() {};
		inline ScrollWindow(D2dWinArgs const& args, 
			Color const& bg = Colors::WhiteSmoke()) : 
			D2dWindow1(args), bgcolor_(bg) {};
		virtual ~ScrollWindow() {};

		Windows::UI::Color bgcolor_ = Colors::WhiteSmoke();
		SpriteVisual visualbg_{ nullptr };

		wunique_ptr<Win32Window> target_;

		wunique_ptr<ScrollNotch> notch_;
		wunique_ptr<Scrollbar> vscroll_;
		wunique_ptr<Scrollbar> hscroll_;

		virtual void SetTarget(Win32Window* target)
		{
			target_ = wunique_ptr<Win32Window>(target);
		}

		bool VScrollup();
		bool VScrolldown();
		bool HScrollup();
		bool HScrolldown();

		//update scrollbar
		virtual void UpdateScroll();
		virtual void UpdateScrollbar();

		void OnScrollChanged(int newcl, bool bHorizontal);

	protected:

		CRECT<LONG> targetRect_;
		CRECT<LONG> clientRect_;

		bool isVScrolling_ = false;
		bool isHScrolling_ = false;

		virtual void WINAPI ScrollTarget(int x, int y);
#if 0
		virtual void CALLBACK OnTargetSize(int width, int height)
		{
			UpdateScroll();
			UpdateScrollbar();
		}
		virtual void CALLBACK OnTargetMove(int x, int y) 
		{
			UpdateScroll();
			UpdateScrollbar();
		}
#endif
		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual void CALLBACK OnClose1() override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual LRESULT CALLBACK OnDpichangedAfterparent() override;
		virtual BOOL CALLBACK OnMouseWheel(WPARAM wp, LPARAM lp) override;
		virtual LRESULT CALLBACK UserMsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};

	class NATIVEWINDOWS2_API ClientScroll : public ScrollWindow
	{
	public:

		inline ClientScroll() {};
		
		inline ClientScroll(D2dWinArgs const& args,
			Color const& bg = Colors::WhiteSmoke()) :
			ScrollWindow(args, bg) {};

		virtual ~ClientScroll() {};

		int tminw_ = 0 , tminh_ = 0;

		virtual void SetTarget(Win32Window* target) override
		{
			// target must be D2dWindow1
			auto temp = static_cast<D2dWindow1*>(target);
			temp->SetD2dArgs({ nullptr, this, thread_ });
			temp->MinSize(&tminw_, &tminh_);
			target_ = wunique_ptr<Win32Window>(target);
		}

	protected:

		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;

	};
}
