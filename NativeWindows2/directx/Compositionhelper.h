#pragma once

#include <NativeWindows2/template/templates.h>
#include <NativeWindows2/directx/IndependentRes.h>

namespace NativeWindows::CompositionHelper
{
	using namespace winrt;
	using namespace Windows::System;
	using namespace Windows::Graphics;
	using namespace Windows::UI;
	using namespace Windows::UI::Composition;
	using namespace Windows::UI::Composition::Desktop;
	namespace abicomp = ABI::Windows::UI::Composition;

	enum : INT32
	{
		VIRTUALSURFACE_MAXPIXEL = 2 << 23,
	};

	inline SpriteVisual CreateColorVisual(
		IN Composition::Compositor& comp,
		IN Windows::UI::Color const& color)
	{
		auto visual = comp.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		auto brush = comp.CreateColorBrush(color);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateColorVisual(
		IN Composition::Compositor& comp,
		IN Windows::UI::Color const& color,
		float w, float h)
	{
		auto visual = comp.CreateSpriteVisual();
		visual.Size({ w, h });

		auto brush = comp.CreateColorBrush(color);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateColorVisual1(
		IN Composition::Compositor& comp,
		IN Windows::UI::Color const& color,
		IN Composition::VisualCollection& list)
	{
		auto visual = comp.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		auto brush = comp.CreateColorBrush(color);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline SpriteVisual CreateColorVisual1(
		IN Composition::Compositor& comp,
		IN Windows::UI::Color const& color,
		IN Composition::VisualCollection& list,
		float w, float h)
	{
		auto visual = comp.CreateSpriteVisual();
		visual.Size({ w, h });

		auto brush = comp.CreateColorBrush(color);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline SpriteVisual CreateD2dVisual(
		IN IndependentRes* res,
		OUT CompositionDrawingSurface& surface,
		CompositionStretch stretch = CompositionStretch::None)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		surface = CreateDrawingSurface(res->graphics_, 1, 1);
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(stretch);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateD2dVisual(
		IN IndependentRes* res,
		OUT CompositionDrawingSurface& surface, 
		float w, float h,
		CompositionStretch stretch = CompositionStretch::None)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.Size({ w, h });

		surface = CreateDrawingSurface(res->graphics_, 1, 1);
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(stretch);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateD2dVisual1(
		IN IndependentRes* res,
		OUT CompositionDrawingSurface& surface,
		IN Composition::VisualCollection& list,
		CompositionStretch stretch = CompositionStretch::None)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		surface = CreateDrawingSurface(res->graphics_, 1, 1);
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(stretch);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline SpriteVisual CreateD2dVisual1(
		IN IndependentRes* res,
		OUT CompositionDrawingSurface& surface,
		IN Composition::VisualCollection& list,
		float w, float h,
		CompositionStretch stretch = CompositionStretch::None)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.Size({ w, h });

		surface = CreateDrawingSurface(res->graphics_, 1, 1);
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(stretch);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline SpriteVisual CreateD2dVirtualSurface(
		IN IndependentRes* res,
		OUT CompositionVirtualDrawingSurface& surface)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		surface = CreateVirtualDrawingSurface(res->graphics_,
			{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(CompositionStretch::None);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateD2dVirtualSurface(
		IN IndependentRes* res,
		OUT CompositionVirtualDrawingSurface& surface, 
		float w, float h)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.Size({ w, h });

		surface = CreateVirtualDrawingSurface(res->graphics_,
			{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(CompositionStretch::None);
		visual.Brush(brush);

		return visual;
	}

	inline SpriteVisual CreateD2dVirtualSurface1(
		IN IndependentRes* res,
		OUT CompositionVirtualDrawingSurface& surface,
		IN Composition::VisualCollection& list)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

		surface = CreateVirtualDrawingSurface(res->graphics_,
			{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(CompositionStretch::None);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline SpriteVisual CreateD2dVirtualSurface1(
		IN IndependentRes* res,
		OUT CompositionVirtualDrawingSurface& surface,
		IN Composition::VisualCollection& list,
		float w, float h)
	{
		auto visual = res->compositor_.CreateSpriteVisual();
		visual.Size({ w, h });

		surface = CreateVirtualDrawingSurface(res->graphics_,
			{ VIRTUALSURFACE_MAXPIXEL, VIRTUALSURFACE_MAXPIXEL });
		auto brush = res->compositor_.CreateSurfaceBrush(surface);
		brush.Stretch(CompositionStretch::None);
		visual.Brush(brush);

		list.InsertAtTop(visual);

		return visual;
	}

	inline auto BeginDraw(
		IN CompositionDrawingSurface& surface, 
		IN RECT const* rect = nullptr)
	{
		ID2D1DeviceContext* d2dc = nullptr;

		auto& size = surface.Size();
		if (size.Width > 0 && size.Height > 0)
		{
			POINT offset;
			auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
			HR(interop->BeginDraw(rect, __uuidof(ID2D1DeviceContext), (void**)&d2dc, &offset));
			d2dc->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset.x, (FLOAT)offset.y));
		}

		return d2dc;
	}

	inline void EndDraw(IN CompositionDrawingSurface& surface)
	{
		auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
		HR(interop->EndDraw());
	}

	inline bool BeginDraw1(
		IN ID2D1DeviceContext6* dc,
		IN D2D1_BITMAP_PROPERTIES1& props,
		IN CompositionDrawingSurface& surface, 
		IN RECT const* rect = nullptr)
	{
		auto& size = surface.Size();
		if (size.Width > 0 && size.Height > 0)
		{
			POINT offset;
			IDXGISurface2* dxsurface;
			ID2D1Bitmap1* bm;

			auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
			HR(interop->BeginDraw(rect, __uuidof(dxsurface), (void**)&dxsurface, &offset));
			HR(dc->CreateBitmapFromDxgiSurface(dxsurface, props, &bm));
			dc->SetTarget(bm); //ref count 증가 하겠지?
			bm->Release(); // 디버깅 필요
			dxsurface->Release(); // 디버깅 필요
			dc->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)offset.x, (FLOAT)offset.y));
			dc->BeginDraw();

			return true;
		}
		else
		{
			return false;
		}
	}

	inline void EndDraw1(
		IN ID2D1DeviceContext6* dc,
		IN CompositionDrawingSurface& surface)
	{
		auto interop = surface.as<abicomp::ICompositionDrawingSurfaceInterop>();
		dc->EndDraw();
		HR(interop->EndDraw());
		dc->SetTarget(nullptr); // 디버깅 필요, 타겟 레프카운트 0으로 해제되는지 확인
	}
}