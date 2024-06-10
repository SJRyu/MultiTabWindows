#pragma once

#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/windows/WindowProperties.h>
#include <NativeWindows2/composition/Cvobj.h>

namespace NativeWindows
{
	class Win32UIThread;

	class NATIVEWINDOWS2_API Cpannel : public Win32Window, public CvRoot
	{
	public:
		inline Cpannel() {}

		inline Cpannel(WinArgs const& args) : Win32Window(args){}

		virtual ~Cpannel()
		{
		}

	protected:

		virtual void Bind();

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) { return 0; };
		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override
		{
			Bind();
			return OnCreate1(createstr);
		}

	};

}
