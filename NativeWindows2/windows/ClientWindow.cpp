#include <pch.h>
#include <NativeWindows2/windows/ClientWindow.h>
#include <NativeWindows2/windows/TabWindow.h>
#include <NativeWindows2/windows/Ctab.h>
#include <NativeWindows2/windows/ContainerWindow.h>
#include <NativeWindows2/Win32UIThread.h>
#include <NativeWindows2/MainLoop.h>

ClientWindow::ClientWindow(TabWindow* tab) :
	Win32Window({ 0, tab->refcontainer_ }), 
	tab_(tab), container_(tab->refcontainer_)
{

}

ClientWindow::~ClientWindow()
{

}

void ClientWindow::CreateEx()
{
	rect_.left = 0;
	rect_.right = container_->rect_.width;
	rect_.top = ncm_->captionh;
	rect_.bottom = container_->rect_.height - rect_.top;
	Win32Window::CreateEx();
}

HWND ClientWindow::SetParent(Win32Window* parent)
{
	auto ret = Win32Window::SetParent(parent);
	if (ret != NULL)
	{
		container_ = (ContainerWindow*)parent;
	}
	return ret;
}

void ClientWindow::OnClose()
{
	if (content_)
	{
		content_->WaitClose();
		delete content_;
		content_ = nullptr;
	}
	cthread_.reset();
}

LRESULT ClientWindow::OnCreate(LPCREATESTRUCT createstr)
{
	cthread_ = std::make_unique<Win32UIThread>();
	cthread_->OnThreadStarts = [this](Win32UIThread*)
	{
		RECT rc = { 0, 0, rect_.width, rect_.height };
		content_->SetWindowArgs({ &rc, this, cthread_.get() });
		content_->CreateEx();
	};
	cthread_->Start1();

	return 0;
}

LRESULT ClientWindow::OnSize(WPARAM state, int width, int height)
{
	::PostMessage(content_->hwnd_, UM_CLIENT_RESIZE, (WPARAM)width, (LPARAM)height);
	return 0;
}

LRESULT ClientWindow1::OnCreate(LPCREATESTRUCT createstr)
{
	RECT rc{ 0, 0, rect_.width, rect_.height };
	content_->SetWindowArgs(WinArgs{ &rc, this, content_->thread_ });
	content_->CreateEx1();

	return 0;
}
