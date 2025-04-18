#pragma once

#include <NativeWindows2/windows/D2dWindow.h>

namespace NativeWindows
{
	class NATIVEWINDOWS2_API CButton : public D2dWindow1
	{
	public:

		CButton(
			WinArgs const& args,
			Windows::UI::Color const& bg = Color{ 0xFF, 0, 0, 0 });
		virtual ~CButton();

		Windows::UI::Color Bgcolor() { return bg_; }
		virtual void Bgcolor(Windows::UI::Color const& bg);

		std::function<int(CButton*)> OnSetup = [](CButton*) { return 0; };
		std::function<void(CButton*)> OnBtnClose = [](CButton*) {};
		std::function<void(CButton*, ID2D1DeviceContext*, int, int)> OnRedraw = [](CButton*, ID2D1DeviceContext*, int w, int h) {};
		std::function<void(CButton*)> OnClick = [](CButton* sender) {
			::PostMessage(sender->parent_->hwnd_, UM_BTNCLICKED, (WPARAM)sender, 0);
		};

	protected:

		Windows::UI::Color bg_;

		ContainerVisual visualb_{ nullptr };
		SpriteVisual visualf_{ nullptr };
		CompositionDrawingSurface surfacef_{ nullptr };
		ScalarKeyFrameAnimation hoveranime_{ nullptr };
		hoverstatus hoverstat_ = hover_inactivate;

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override;
		virtual LRESULT CALLBACK OnPaint() override;
		virtual LRESULT CALLBACK OnLbtndown(int state, int x, int y) override;
		virtual LRESULT CALLBACK OnLbtnup(int state, int x, int y) override;
		virtual LRESULT CALLBACK OnMousemove(int state, int x, int y) override;
		virtual LRESULT CALLBACK OnMouseleave() override;

		virtual void CALLBACK OnClose1() override
		{
			OnBtnClose(this);
#ifdef WINRT_CLOSETEST
			hoveranime_.Close();
			surfacef_ = nullptr;
			auto br = visualf_.Brush();
			visualf_.Brush(nullptr);
			br.Close();
#endif
		}

		virtual void CALLBACK Status(hoverstatus activate);
		virtual void CALLBACK Redraw(LONG w, LONG h);

	};

	class NATIVEWINDOWS2_API CButton1 : public CButton
	{
	public:

		CButton1(WinArgs const& args);
		virtual ~CButton1();

		virtual void [[deprecated]] Bgcolor(Windows::UI::Color const& bg) override { /* no effect */ };

		std::function<void(CButton1*, ID2D1DeviceContext*, int, int)> OnRedrawBg = [](CButton1*, ID2D1DeviceContext*, int w, int h) {};

	protected:

		CompositionDrawingSurface surfaceb_{ nullptr };

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual void CALLBACK Redraw(LONG w, LONG h) override;

		virtual void CALLBACK OnClose1() override
		{
			OnBtnClose(this);
#ifdef WINRT_CLOSETEST
			hoveranime_.Close();
			surfacef_ = nullptr;
			auto br = visualf_.Brush();
			visualf_.Brush(nullptr);
			br.Close();

			surfaceb_ = nullptr;
			auto vb = visualb_.as<SpriteVisual>();
			auto br1 = vb.Brush();
			vb.Brush(nullptr);
			br1.Close();
#endif
		}
	};

	class CEllipseButton : public CButton
	{
	public:

		CEllipseButton(WinArgs const& args,
			Windows::UI::Color const& bg = Color{ 0xFF, 0, 0, 0 });
		virtual ~CEllipseButton();

		virtual void Bgcolor(Windows::UI::Color const& bg) override;

	protected:

		//ShapeVisual svisualb_{ nullptr };
		CompositionEllipseGeometry geob_{ nullptr };
		CompositionSpriteShape shapeb_{ nullptr };

		virtual LRESULT CALLBACK OnCreate1(LPCREATESTRUCT createstr) override;
		virtual void CALLBACK Redraw(LONG w, LONG h) override;

		virtual LRESULT CALLBACK OnSize(WPARAM state, int width, int height) override
		{
			/*
			After a successful call to SetWindowRgn, the system owns the region specified by the region handle hRgn.
			The system does not make a copy of the region. Thus, you should not make any further function calls with this region handle.
			In particular, do not delete this region handle. The system deletes the region handle when it no longer needed.
			*/
			auto region = ::CreateEllipticRgn(0, 0, rect_.width, rect_.height);
			::SetWindowRgn(hwnd_, region, false);
			return CButton::OnSize(state, width, height);
		}

		virtual void CALLBACK OnClose1() override
		{
			OnBtnClose(this);
#ifdef WINRT_CLOSETEST
			hoveranime_.Close();
			surfacef_ = nullptr;
			auto br = visualf_.Brush();
			visualf_.Brush(nullptr);
			br.Close();
			
			auto vb = visualb_.as<ShapeVisual>();
			vb.Shapes().Clear();
			shapeb_.Geometry(nullptr);
			shapeb_.Close();
			geob_.Close();
#endif
		}
	};
}