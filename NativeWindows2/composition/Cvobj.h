#pragma once

#include <NativeWindows2/composition/InputSync.h>
#include <NativeWindows2/template/templates.h>
#include <NativeWindows2/directx/Compositionhelper.h>
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

		//staple dc for svg and textwriter
		com_ptr<ID2D1DeviceContext6> bitmapdc_;
		ID2D1DeviceContext6* refBitmapDc_ = nullptr;
		D2D1_BITMAP_PROPERTIES1 bmprops_;

		void CreateBitmapDc();

		inline ShapeVisual AddShapeVisual()
		{
			auto visual = refres_->compositor_.CreateShapeVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline ShapeVisual AddShapeVisual(float w, float h)
		{
			auto visual = refres_->compositor_.CreateShapeVisual();
			visual.Size({ w, h });
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddColorVisual(Windows::UI::Color const& color)
		{
			return CompositionHelper::CreateColorVisual1(refres_->compositor_, color, visuals_);
		}

		inline SpriteVisual AddColorVisual(Windows::UI::Color const& color, float w, float h)
		{
			return CompositionHelper::CreateColorVisual1(refres_->compositor_, color, visuals_, w, h);
		}

		inline SpriteVisual AddD2dVisual(
			OUT CompositionDrawingSurface& surface,
			CompositionStretch stretch = CompositionStretch::None)
		{
			return CompositionHelper::CreateD2dVisual1(refres_, surface, visuals_, stretch);
		}

		inline SpriteVisual AddD2dVisual(
			OUT CompositionDrawingSurface& surface, float w, float h,
			CompositionStretch stretch = CompositionStretch::None)
		{
			return CompositionHelper::CreateD2dVisual1(refres_, surface, visuals_, w, h, stretch);
		}

		inline SpriteVisual AddD2dVirtualSurface(
			OUT CompositionVirtualDrawingSurface& surface)
		{
			return CompositionHelper::CreateD2dVirtualSurface1(refres_, surface, visuals_);
		}

		inline SpriteVisual AddD2dVirtualSurface(
			OUT CompositionVirtualDrawingSurface& surface, float w, float h)
		{
			return CompositionHelper::CreateD2dVirtualSurface1(refres_, surface, visuals_, w, h);
		}

		inline auto BeginDraw(IN CompositionDrawingSurface& surface, IN RECT const* rect = nullptr)
		{
			return CompositionHelper::BeginDraw(surface, rect);
		}

		inline void EndDraw(IN CompositionDrawingSurface& surface)
		{
			CompositionHelper::EndDraw(surface);
		}

		inline auto BeginDraw1(IN CompositionDrawingSurface& surface, IN RECT const* rect = nullptr)
		{
			auto dc = refBitmapDc_;

			if (!CompositionHelper::BeginDraw1(dc, bmprops_, surface, rect))
			{
				dc = nullptr;
			}

			return dc;
		}

		inline void EndDraw1(CompositionDrawingSurface& surface)
		{
			CompositionHelper::EndDraw1(refBitmapDc_, surface);
		}
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

	class NATIVEWINDOWS2_API CvChild : public Cvi
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

		template <typename T>
		inline auto AnimeMoveVisuals(T x, T y)
		{
			animemove_.InsertKeyFrame(1.0f, { (float)x, (float)y, 0.0f });
			rootv_.StartAnimation(L"Offset", animemove_);
		}

		template <typename T>
		inline auto AnimeSizeVisuals(T w, T h)
		{
			animesize_.InsertKeyFrame(1.0f, { (float)w, (float)h });
			rootv_.StartAnimation(L"Size", animesize_);
		}

		inline auto PlaceAtTop()
		{
			parentv_.Remove(rootv_);
			parentv_.InsertAtTop(rootv_);
		}

		virtual Cvi* WINAPI HitTest(UINT msg, WPARAM wp, LPARAM lp) override;

	protected:

	};

	class NATIVEWINDOWS2_API CvRoot : public Cvi
	{
	public:

		DesktopWindowTarget target_{ nullptr };

		Cvi* mfocused_ = nullptr;

		void WINAPI Bind(Win32Window* win);

		virtual Cvi* WINAPI HitTest(UINT msg, WPARAM wp, LPARAM lp) override;

	protected:

	};

}
