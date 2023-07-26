#pragma once

#include <NativeWindows2/windows/Win32Window.h>

namespace NativeWindows
{
	class Win32UIThread;
	class TabWindow;
	class ContainerWindow;

	class NATIVEWINDOWS2_API ClientWindow : public Win32Window
	{
	public:
		ClientWindow(TabWindow* tab);
		virtual ~ClientWindow();

		std::unique_ptr<Win32UIThread> cthread_;
		TabWindow* tab_;
		ContainerWindow* container_;

		inline void SetContent(Win32Window* win) { content_ = win; };
		inline auto GetContent() { return content_; }
		
		virtual void CreateEx() override;
		virtual HWND SetParent(Win32Window* parent) override;

		virtual void WINAPI ShowWindow(int nCmdShow = SW_SHOW) override
		{
			OnShowWindow(this, nCmdShow);
			Win32Window::ShowWindow(nCmdShow);
		}

		std::function<void(ClientWindow*, int)> OnShowWindow = 
			[](ClientWindow*, int) {};

	protected:

		Win32Window* content_ = nullptr;

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual void CALLBACK OnClose() override;

	};

	class NATIVEWINDOWS2_API ClientWindow1 : public ClientWindow
	{
	public:
		ClientWindow1(TabWindow* tab) : ClientWindow(tab) {}
		virtual ~ClientWindow1() {}

	protected:

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override;
	};
}
