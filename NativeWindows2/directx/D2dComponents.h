#pragma once

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

		DesktopWindowTarget target_{ nullptr };
		VisualCollection parentv_{ nullptr };

		ContainerVisual rootv_{ nullptr };
		ContainerVisual topv_{ nullptr };
		ContainerVisual bottomv_{ nullptr };

		VisualCollection visuals_{ nullptr };

		Vector3KeyFrameAnimation animemove_{ nullptr };
		Vector2KeyFrameAnimation animesize_{ nullptr };

		void InitCompositor();
		void InitCompositor1(D2dComponents* parent);

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

		inline ShapeVisual AddShapeVisual1(IN ContainerVisual& parent)
		{
			auto visual = refres_->compositor_.CreateShapeVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });
			parent.Children().InsertAtTop(visual);

			return visual;
		}

		inline ShapeVisual AddShapeVisual1(IN ContainerVisual& parent, float w, float h)
		{
			auto visual = refres_->compositor_.CreateShapeVisual();
			visual.Size({ w, h });
			parent.Children().InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddColorVisual(Windows::UI::Color const& color)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

			auto brush = refres_->compositor_.CreateColorBrush(color);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddColorVisual(Windows::UI::Color const& color, float w, float h)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.Size({ w, h });

			auto brush = refres_->compositor_.CreateColorBrush(color);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddColorVisual1(
			IN ContainerVisual& parent, Color const& color)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

			auto brush = refres_->compositor_.CreateColorBrush(color);

			visual.Brush(brush);
			parent.Children().InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddColorVisual1(
			IN ContainerVisual& parent, Color const& color, float w, float h)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.Size({ w, h });

			auto brush = refres_->compositor_.CreateColorBrush(color);

			visual.Brush(brush);
			parent.Children().InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddD2dVisual(
			OUT CompositionDrawingSurface& surface, 
			CompositionStretch stretch = CompositionStretch::None)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

			surface = CreateDrawingSurface(refres_->graphics_, 1, 1);
			auto brush = refres_->compositor_.CreateSurfaceBrush(surface);
			brush.Stretch(stretch);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddD2dVisual(
			OUT CompositionDrawingSurface& surface, float w, float h,
			CompositionStretch stretch = CompositionStretch::None)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.Size({ w, h });

			surface = CreateDrawingSurface(refres_->graphics_, 1, 1);
			auto brush = refres_->compositor_.CreateSurfaceBrush(surface);
			brush.Stretch(stretch);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddD2dVirtualSurface(
			OUT CompositionVirtualDrawingSurface& surface)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

			surface = CreateVirtualDrawingSurface(refres_->graphics_,
				{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
			auto brush = refres_->compositor_.CreateSurfaceBrush(surface);
			brush.Stretch(CompositionStretch::None);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline SpriteVisual AddD2dVirtualSurface(
			OUT CompositionVirtualDrawingSurface& surface, float w, float h)
		{
			auto visual = refres_->compositor_.CreateSpriteVisual();
			visual.Size({ w, h });
			
			surface = CreateVirtualDrawingSurface(refres_->graphics_, 
				{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
			auto brush = refres_->compositor_.CreateSurfaceBrush(surface);
			brush.Stretch(CompositionStretch::None);
			visual.Brush(brush);
			visuals_.InsertAtTop(visual);

			return visual;
		}

		inline auto BeginDraw(IN CompositionDrawingSurface& surface)
		{
			ID2D1DeviceContext* d2dc = nullptr;

			auto& size = surface.Size();
			if (size.Width > 0 && size.Height > 0)
			{
				auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
				HR(interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void**)&d2dc, &offset_));
				d2dc->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset_.x, (FLOAT)offset_.y));
			}
		
			return d2dc;
		}

		inline auto BeginDraw(IN CompositionDrawingSurface& surface, IN RECT const* rect)
		{
			ID2D1DeviceContext* d2dc = nullptr;

			auto& size = surface.Size();
			if (size.Width > 0 && size.Height > 0)
			{
				auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
				HR(interop->BeginDraw(rect, __uuidof(ID2D1DeviceContext), (void**)&d2dc, &offset_));
				d2dc->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset_.x, (FLOAT)offset_.y));
			}

			return d2dc;
		}

		inline void EndDraw(CompositionDrawingSurface& surface)
		{
			auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
			HR(interop->EndDraw());
		}

		inline auto BeginDraw1(CompositionDrawingSurface& surface)
		{
			ID2D1DeviceContext6* dc = nullptr;
			auto& size = surface.Size();
			if (size.Width > 0 && size.Height > 0)
			{
				auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
				HR(interop->BeginDraw(nullptr, __uuidof(bmsurface_.get()), bmsurface_.put_void(), &offset_));

				HR(bitmapdc_->CreateBitmapFromDxgiSurface(bmsurface_.get(), bmprops_, bitmap_.put()));
				bitmapdc_->SetTarget(bitmap_.get());
				bitmapdc_->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset_.x, (FLOAT)offset_.y));
				bitmapdc_->BeginDraw();

				dc = bitmapdc_.get();
			}
			return dc;
		}

		inline auto BeginDraw1(CompositionDrawingSurface& surface, IN RECT const* rect)
		{
			ID2D1DeviceContext6* dc = nullptr;
			auto& size = surface.Size();
			if (size.Width > 0 && size.Height > 0)
			{
				auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
				HR(interop->BeginDraw(rect, __uuidof(bmsurface_.get()), bmsurface_.put_void(), &offset_));

				HR(bitmapdc_->CreateBitmapFromDxgiSurface(bmsurface_.get(), bmprops_, bitmap_.put()));
				bitmapdc_->SetTarget(bitmap_.get());
				bitmapdc_->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset_.x, (FLOAT)offset_.y));
				bitmapdc_->BeginDraw();

				dc = bitmapdc_.get();
			}
			return dc;
		}

		inline void EndDraw1(CompositionDrawingSurface& surface)
		{
			auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
			bitmapdc_->EndDraw();
			HR(interop->EndDraw());
			bitmapdc_->SetTarget(nullptr);
			bitmap_ = nullptr;
			bmsurface_ = nullptr;
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

		POINT offset_;
		D2D1::Matrix3x2F mat_;

		com_ptr<ID2D1DeviceContext6> bitmapdc_;
		com_ptr<IDXGISurface2> bmsurface_;
		com_ptr<ID2D1Bitmap1> bitmap_;
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
