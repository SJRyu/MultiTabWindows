#include <pch.h>
#include <NativeWindows2/windows/TabWindow.h>
#include <NativeWindows2/windows/Ctab.h>
#include <NativeWindows2/windows/CButton.h>
#include <NativeWindows2/windows/ContainerWindow.h>
#include <NativeWindows2/windows/ClientWindow.h>
#include <NativeWindows2/MainLoop.h>

TabWindow::TabWindow(Ctab* parent) :
	ctab_(parent), refcontainer_(parent->refcontainer_),
	D2dWindow1({ 0, parent })
{
	assert(parent_ != nullptr);
}

TabWindow::~TabWindow()
{
}

void TabWindow::SetContent(Win32Window* content)
{
	client_->SetContent(content);
}

VOID TabWindow::OnClose1()
{
	OnTabClose(this);
	btnclose_.reset();
	client_.reset();
#ifdef WINRT_CLOSETEST
	shadow_.Close();
	hoveranime_.Close();
	
	txtsurface_ = nullptr;
	auto br = txtvisual_.Brush();
	txtvisual_.Brush(nullptr);
	br.Close();

	bgsurface_ = nullptr;
	auto br1 = bgvisual_.Brush();
	bgvisual_.Brush(nullptr);
	br1.Close();
#endif
}

void TabWindow::CreateEx()
{
	rect_.left = (int)ceil(ctab_->tabwidth_ * position_);
	rect_.top = 0;
	rect_.width = (int)ctab_->tabwidth_;
	rect_.height = ncm_->tabh;

	//D2dWindow1::CreateEx();
	D2dWindow::CreateEx();
}

void TabWindow::SetClient()
{
	client_ = wmake_unique<ClientWindow1>(this);
}

LRESULT TabWindow::OnCreate1(LPCREATESTRUCT createstr)
{
	float w = (float)rect_.width, h = (float)rect_.height;

	rootv_.Offset({ (float)rect_.left, h, 0.0f });

	bgvisual_ = AddD2dVisual(bgsurface_);

	LONG hhh = (LONG)h / 3;
	LONG off = ncm_->padded;

	RECT rect{ 0, off, (LONG)w - off, 0};
	rect.left = rect.right - hhh;
	rect.bottom = rect.top + hhh;

	btnclose_ = wmake_unique<CEllipseButton>(
		D2dWinArgs{ &rect, this }, Color{ 0xFF, 255, 255, 153 });

	btnclose_->OnSetup = [](CButton* btn) -> int
	{
		btn->rootv_.Opacity(0.0f);
		auto animation = btn->refres_->compositor_.CreateScalarKeyFrameAnimation();
		animation.InsertKeyFrame(1.0f, 1.0f);
		using timespan = std::chrono::duration<int, std::ratio<1, 1000>>;
		animation.Duration(timespan(400));
		animation.DelayTime(timespan(400));
		btn->rootv_.StartAnimation(L"Opacity", animation);
		return 0;
	};
	
	btnclose_->OnRedraw = [this](CButton* btn, ID2D1DeviceContext* dc, int w, int h)
	{
		auto d1e0 = ncm_->d1e0;
		auto d2e0 = ncm_->d2e0;

		ID2D1SolidColorBrush* brush;
		dc->CreateSolidColorBrush(D2D1::ColorF(0x808060), &brush);
		dc->DrawLine({ d2e0, d2e0 }, { w - d2e0, h - d2e0 },
			brush, d1e0);
		dc->DrawLine({ w - d2e0, d2e0 }, { d2e0, h - d2e0 },
			brush, d1e0);
		brush->Release();
	};
	btnclose_->CreateEx();
	btnclose_->ShowWindow();

	txtvisual_ = AddD2dVisual(txtsurface_);

	fontaxis_.axisTag = DWRITE_FONT_AXIS_TAG_WEIGHT;
	fontaxis_.value = (FLOAT)DWRITE_FONT_WEIGHT_SEMI_LIGHT;

	refres_->dwriteFactory_->CreateTextFormat(
		L"Cambria", nullptr,
		&fontaxis_, 1,
		h / 2,
		L"ko_KR", textformat_.put());

	textformat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textformat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	
	DWRITE_TRIMMING trimming{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
	textformat_->SetTrimming(&trimming, nullptr);

	stgradient_[0].color = D2D1::ColorF(textrgb_, 1.0f);
	stgradient_[0].position = 0.0f;
	stgradient_[1].color = D2D1::ColorF(textrgb_, 0.85f);
	stgradient_[1].position = 0.78f;
	stgradient_[2].color = D2D1::ColorF(textrgb_, 0.0f);
	stgradient_[2].position = 0.98f;

	hoveranime_ = refres_->compositor_.CreateScalarKeyFrameAnimation();
	using timespan = std::chrono::duration<int, std::ratio<1, 1000>>;
	hoveranime_.Duration(timespan(400));
	
	shadow_ = refres_->compositor_.CreateDropShadow();
	shadow_.BlurRadius(h / 2);
	shadow_.Offset({ 0, h / 4, 0 });
	shadow_.Color(Color{ 0xFF, 0xA0, 0xA0, 0x40 });
	bgvisual_.Shadow(shadow_);

	SetClient();
	OnSetup(this);
	client_->CreateEx();

	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd_, NULL, g_hinst, NULL);

	::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Set up "tool" information. In this case, the "tool" is the entire parent window.

	TOOLINFO ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd_;
	ti.hinst = g_hinst;
	ti.lpszText = (LPWSTR)title_.c_str();

	GetClientRect(hwnd_, &ti.rect);

	// Associate the tooltip with the "tool" window.
	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	AnimeMoveVisuals(rect_.left, rect_.top);
	SetActivate(hover_activate);

	return 0;
}

LRESULT TabWindow::OnDpichangedAfterparent()
{
	float h = (float)ncm_->tabh;

	textformat_ = nullptr;
	refres_->dwriteFactory_->CreateTextFormat(
		L"Cambria", nullptr,
		&fontaxis_, 1,
		(FLOAT)h / 2,
		L"ko_KR", textformat_.put());

	//dbg_msg("ondpi h = %f", h);

	textformat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textformat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	DWRITE_TRIMMING trimming{ DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
	textformat_->SetTrimming(&trimming, nullptr);

	RefreshView();

	int hhh = (int)(h / 3);
	int off = ncm_->padded;

	btnclose_->SetWindowPos(0,
		0, 0, hhh, hhh,
		SWP_NOMOVE | SWP_NOZORDER);

	shadow_.BlurRadius((float)h/2);
	shadow_.Offset({ 0, (float)h/4, 0 });
	return 0;
}

LRESULT TabWindow::OnSize(WPARAM state, int width, int height)
{
	if (width <= 0 || height <= 0) return 0;

	int off = ncm_->padded;
	int hhh = height / 3;
	btnclose_->SetWindowPos(0, 
		width - off - hhh, off, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER);

	RefreshView();

	return 0;
}

LRESULT TabWindow::OnMove(int x, int y)
{
	if (dragging_)
	{
		int org = (int)(ctab_->tabwidth_ * position_);
		int thr = (int)(ctab_->tabwidth_ / 2);

		if (x < org - thr)
		{
			ctab_->SwapTab(position_, position_ - 1);
		}
		else if (x > org + thr)
		{
			ctab_->SwapTab(position_, position_ + 1);
		}
	}

	return 0;
}

LRESULT TabWindow::OnLbtndown(int state, int x, int y)
{
	if (ctab_->tabs_.size() == 1)
	{
		refcontainer_->isDragbyTab_ = true;
		::SendMessage(refcontainer_->hwnd_, WM_SYSCOMMAND, SC_DRAGMOVE, 0);
		return 0;
	}

	capturex_ = x;
	capturey_ = y;

	ctab_->ActiveTab(position_);

	::SetCapture(hwnd_);
	dragging_ = true;

	return 0;
}

LRESULT TabWindow::OnLbtnup(int state, int x, int y)
{
	StopDragging();
	return 0;
}

LRESULT TabWindow::OnMousemove(int state, int x, int y)
{
	if (status_ == hover_inactivate)
	{
		SetActivate(hover_hovering);
	}
	else if (dragging_)
	{
		int dx = x - capturex_;
		int dy = y - capturey_;

		// split windows
		if (abs(dy) > (ncm_->captionh + ncm_->hframe))
		{
			POINT pt{ x, y };
			::ClientToScreen(hwnd_, &pt);
			RECT rc{ pt.x, pt.y, pt.x, pt.y };

			auto& crect = refcontainer_->xRect_;
			rc.left = pt.x - capturex_ - ncm_->vframe;
			rc.right = rc.left + crect.width;
			rc.top = pt.y - capturey_ - ncm_->taby;
			rc.bottom = rc.top + crect.height;

			ctab_->MoveoutTab(this);
			auto ml = (MainLoop*)thread_;
			auto container = ml->NewContainer(rc);
			container->ctab_->InsertTab(this);

			pt = { capturex_, capturey_ };
			::ClientToScreen(hwnd_, &pt);
			rc = { pt.x, pt.y, pt.x, pt.y };
			::ClipCursor(&rc);

			refcontainer_->isDragbyTab_ = true;
			::SendMessage(refcontainer_->hwnd_, WM_SYSCOMMAND, SC_DRAGMOVE, 0);
			::ClipCursor(NULL);
		}
		else
		{
			int mv = std::max((int)rect_.left + dx, 0);

			mv = std::min((int)ctab_->tabend_ - (int)ctab_->tabwidth_, mv);

			SetWindowPos(HWND_TOP, mv, rect_.top, 0, 0,
				SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW1);
		}
	}

	return 0;
}

HWND TabWindow::SetParent(Win32Window* parent)
{
	auto ret = D2dWindow1::SetParent(parent);
	if (ret != NULL)
	{
		ctab_ = (Ctab*)parent;
		refcontainer_ = ctab_->refcontainer_;
		ret = client_->SetParent(refcontainer_);
	}
	return ret;
}


LRESULT TabWindow::OnMouseleave()
{
	if (status_ == hover_hovering)
	{
		SetActivate(hover_inactivate);
	}
	return 0;
}

LRESULT TabWindow::OnCButtonClicked(WPARAM wp, LPARAM lp)
{
	ctab_->PostRemove(this);
	return 0;
}

void TabWindow::Reposition()
{
	int w = (int)ctab_->tabwidth_;
	int h = ncm_->tabh;
	if (w <= 0) return;

	AnimeWindowPos(0,
		(int)ceil(ctab_->tabwidth_ * position_), 0,
		w, h,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);
}

void TabWindow::ResizeClient()
{
	client_->SetWindowPos(0,
		0, ncm_->captionh,
		refcontainer_->rect_.width,
		refcontainer_->rect_.height - ncm_->captionh,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW1);
}

void TabWindow::SetActivate(hoverstatus stat)
{
	if (stat == status_) return;

	switch (stat)
	{
	case hover_activate:
	{
		PlaceAtTop();
		hoveranime_.InsertKeyFrame(1.0f, 1.0f);
		bgvisual_.StartAnimation(L"Opacity", hoveranime_);
		txtvisual_.StartAnimation(L"Opacity", hoveranime_);

		client_->SetWindowPos(HWND_TOP,
			0, ncm_->captionh,
			refcontainer_->rect_.width,
			refcontainer_->rect_.height - ncm_->captionh,
			SWP_NOACTIVATE | SWP_NOREDRAW1);
		client_->ShowWindow();
	}
	break;
	case hover_hovering:
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd_;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);

		hoveranime_.InsertKeyFrame(1.0f, 0.70f);
		bgvisual_.StartAnimation(L"Opacity", hoveranime_);
		txtvisual_.StartAnimation(L"Opacity", hoveranime_);

		if (status_ == hover_activate)
		{
			client_->ShowWindow(SW_HIDE);
		}
	}
	break;
	case hover_inactivate:
	default:
	{
		if (status_ == hover_activate)
		{
			bgvisual_.Opacity(0.30f);
			txtvisual_.Opacity(0.30f);
		}
		else
		{
			hoveranime_.InsertKeyFrame(1.0f, 0.30f);
			bgvisual_.StartAnimation(L"Opacity", hoveranime_);
			txtvisual_.StartAnimation(L"Opacity", hoveranime_);
		}

		if (status_ == hover_activate)
		{
			client_->ShowWindow(SW_HIDE);
		}
	}
	break;
	}

	status_ = stat;
}


LRESULT TabWindow::OnPaint()
{
	int w = rect_.width;
	int h = rect_.height;

	if (w > 0 && h > 0)
	{
		RedrawFrame(w, h);
		RedrawText(w, h);
	}
	ValidateView();
	return 0;
}

void TabWindow::RedrawFrame(int w, int h)
{
	bgsurface_.Resize({ w, h });
	auto dc = BeginDraw(bgsurface_);
	if (dc)
	{
		dc->Clear();

		ID2D1SolidColorBrush* brush;
		dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &brush);

		FLOAT r = (FLOAT)ncm_->padded;

		//dbg_msg("redraw %f x %f, r = %f, rrect = %f", w, h, r, ncm_->d2e0);

		D2D1_ROUNDED_RECT rrect{ { 0, 0, w - ncm_->d2e0, h + r }, r, r };
		dc->FillRoundedRectangle(rrect, brush);

		brush->Release();
		EndDraw(bgsurface_);
		dc->Release();
	}
}

void TabWindow::RedrawText(int w, int h)
{
	FLOAT hh = (float)h / 2, hhh = (float)h / 3;
	txtsurface_.Resize({ w, h });

	auto dc = BeginDraw(txtsurface_);
	if (dc)
	{
		dc->Clear();

		// doc says default is os setting. but seems not 
		dc->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		dc->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

		FLOAT r = (FLOAT)ncm_->padded;
		D2D1_RECT_F textrect = D2D1::RectF(hhh, 0.0f, w - hhh - r, (FLOAT)h);

		ID2D1LinearGradientBrush* gbrush;
		ID2D1GradientStopCollection* gradientstops;
		HR(dc->CreateGradientStopCollection(
			stgradient_,
			3,
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&gradientstops
		));

		HR(dc->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(
				D2D1::Point2F(0, textrect.bottom),
				D2D1::Point2F(textrect.right, textrect.bottom)),
			gradientstops,
			&gbrush
		));

		if (auto len = title_.size())
		{
			dc->DrawTextW(title_.c_str(), (UINT32)len, textformat_.get(),
				&textrect,
				gbrush);
		}

		gbrush->Release();
		gradientstops->Release();
		EndDraw(txtsurface_);
		dc->Release();
	}
}

void TabWindow::StopDragging()
{
	if (dragging_)
	{
		::ReleaseCapture();
		Reposition();
		dragging_ = false;
	}
}
