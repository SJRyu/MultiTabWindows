#include <pch.h>
#include <Win32Windows/NvrTab.h>
#include <NativeWindows2/windows/ClientWindow.h>
#include <NativeWindows2/windows/Ctab.h>
#include <NativeWindows2/windows/ContainerWindow.h>
#include <Win32Windows/NvrWindow.h>
#include <MyApp.h>

using namespace NativeWindows;

NvrTab::NvrTab(Ctab* parent) : TabWindow(parent)
{
	title_ = L"Viewer ";

	OnSetup = [](TabWindow* sender)
	{
		NvrTab* tab = (NvrTab*)sender;
		auto content = new VideoPannel(tab);
		tab->SetContent(content);

		auto count = MyApp::UpCountTab();
		sender->title_ += std::to_wstring(count);
	};
}

NvrTab::~NvrTab()
{
}

BOOL NvrTab::OnContextMenu(HWND hwnd, int xpos, int ypos)
{
	return TRUE;
}

LRESULT NvrTab::OnCommand(WPARAM wp, LPARAM lp)
{
	return FALSE;
}

LRESULT NvrTab::UserMsgHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case UM_OPEN_CAMSETTING:
	{
		int chn = (int)wParam;


		return 0;
	}
	default:
	{
		break;
	}
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
