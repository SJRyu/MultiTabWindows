#pragma once

#include <NativeWindows2/composition/InputSync.h>
#include <NativeWindows2/template/templates.h>
#include <NativeWindows2/directx/IndependentRes.h>

namespace NativeWindows
{
	using namespace winrt;
	using namespace Windows::System;
	using namespace Windows::Graphics;
	using namespace Windows::UI;
	using namespace Windows::UI::Composition;
	using namespace Windows::UI::Composition::Desktop;
	namespace abicomp = ABI::Windows::UI::Composition;

	class Win32Window;

	class NATIVEWINDOWS2_API Cvobj
	{
	public:
		IndependentRes* refres_ = nullptr;
		Compositor compositor_{ nullptr };

		ContainerVisual rootv_{ nullptr };
		ContainerVisual topv_{ nullptr };
		ContainerVisual bottomv_{ nullptr };
		VisualCollection visuals_{ nullptr };

	};

	class NATIVEWINDOWS2_API Cvi : public Cvobj, public InputSync
	{
	public:
		Cvi* vparent_ = nullptr;
		std::list<Cvi*> vchild_;

		static inline void Bind(Cvi* parent, Cvi* child)
		{
			parent->vchild_.push_front(child);
			child->vparent_ = parent;
		}

		static FORCEINLINE Cvi* HitTest(
			UINT msg, WPARAM wp, LPARAM lp, std::list<Cvi*>& chlist);

		virtual Cvi* HitTest(UINT msg, WPARAM wp, LPARAM lp) = 0;
	};

	class NATIVEWINDOWS2_API Cvobj1 : public Cvi
	{
	public:
		virtual void Bind(Cvi* parent);

		CRECT<LONG> vrect_;

		inline void MoveRect(int x, int y)
		{
			vrect_.MoveToXY(x, y);
		}

		inline void SizeRect(int w, int h)
		{
			vrect_.width = w;
			vrect_.height = h;
		}

		VisualCollection parentv_{ nullptr };
		Vector3KeyFrameAnimation animemove_{ nullptr };
		Vector2KeyFrameAnimation animesize_{ nullptr };

		virtual Cvi* WINAPI HitTest(UINT msg, WPARAM wp, LPARAM lp) override;

	protected:

	};

	class NATIVEWINDOWS2_API CvRoot : public Cvi
	{
	public:

		Cvi* mfocused_ = nullptr;

		void WINAPI Bind(Win32Window* win);

		virtual Cvi* WINAPI HitTest(UINT msg, WPARAM wp, LPARAM lp) override;

	protected:

	};

}
