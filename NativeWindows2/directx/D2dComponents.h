#pragma once

#include <NativeWindows2/template/templates.h>
#include <NativeWindows2/directx/IndependentRes.h>
#include <NativeWindows2/directx/Compositionhelper.h>

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
	class IndependentRes;

	class NATIVEWINDOWS2_API D2dComponents
	{
	public:

		enum : INT32
		{
			VIRTUALSURFACE_MAXPIXEL = 2 << 23,
		};

		Win32Window* window_ = nullptr;
		IndependentRes* refres_ = nullptr;

		DesktopWindowTarget target_{ nullptr }; //useless
		VisualCollection parentv_{ nullptr };

		ContainerVisual rootv_{ nullptr };
		ContainerVisual topv_{ nullptr };
		ContainerVisual bottomv_{ nullptr };

		VisualCollection visuals_{ nullptr };

		Vector3KeyFrameAnimation animemove_{ nullptr };
		Vector2KeyFrameAnimation animesize_{ nullptr };

		void InitCompositor();
		void InitCompositor1(D2dComponents* parent);
		void NewCompParent(D2dComponents* newp);

		void ReleaseCompositor();
		void ReleaseCompositor1();

		inline auto& Compositor()
		{
			return refres_->compositor_;
		}

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

		template <typename T>
		inline auto MoveVisuals(T x, T y)
		{
			rootv_.Offset({ (float)x, (float)y, 0.0f });
		}

		template <typename T>
		inline auto SizeVisuals(T w, T h)
		{
			rootv_.Size({ (float)w, (float)h });
		}

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

		D2D1::Matrix3x2F mat_;

		com_ptr<ID2D1DeviceContext6> bitmapdc_;
		ID2D1DeviceContext6* refBitmapDc_ = nullptr;
		D2D1_BITMAP_PROPERTIES1 bmprops_;

		//DXGI_FORMAT_B8G8R8A8_UNORM
		void CreateBitmapDc();

	protected:

		D2dComponents(Win32Window* window);
		virtual ~D2dComponents();

		D2dComponents() = delete;
		D2dComponents(const D2dComponents& r) = delete;
		D2dComponents& operator=(const D2dComponents& r) = delete;

	};	

}
