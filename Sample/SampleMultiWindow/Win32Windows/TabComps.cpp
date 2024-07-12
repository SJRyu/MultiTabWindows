#include <pch.h>
#include "TabComps.h"
#include "TestCpannel.h"

using namespace NativeWindows;

TabComps::TabComps(Ctab* parent) : TabWindow(parent)
{
	title_ = L"CVisuals";

	OnSetup = [](TabWindow* sender)
	{
		TabComps* tab = (TabComps*)sender;
		auto content = new TestCpannel(tab);
		tab->SetContent(content);
	};
}

