#pragma once

#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/windows/WindowProperties.h>
#include <NativeWindows2/composition/Cvobj.h>

namespace NativeWindows
{
	using namespace Windows::UI::Composition;

	class NATIVEWINDOWS2_API Cpannel : public Win32Window, public CvRoot
	{
	public:
		inline Cpannel() {}

		inline Cpannel(WinArgs const& args) : Win32Window(args){}

		virtual ~Cpannel(){}

		Windows::UI::Color color_ = Windows::UI::Colors::White();
		SpriteVisual bg_{ nullptr };

	protected:

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) { return 0; }

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override
		{ 
			Bind(this);
			bg_ = AddColorVisual(color_);
			return OnCreate1(createstr);
		};
	};

}
