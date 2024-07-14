#include <pch.h>
#include <NativeWindows2/windows/CButton.h>

CButton::CButton(WinArgs const& args, Color const& bg) :
	D2dWindow1(args), bg_(bg)
{
	assert(parent_ != nullptr);
}

CButton::~CButton()
{
}

LRESULT CButton::OnCreate1(LPCREATESTRUCT createstr)
{
	float w = (float)rect_.width;
	float h = (float)rect_.height;

	visualb_ = AddColorVisual(bg_);
	visualb_.Opacity(0.0f);

	visualf_ = AddD2dVisual(surfacef_);

	hoveranime_ = refres_->compositor_.CreateScalarKeyFrameAnimation();
	using timespan = std::chrono::duration<int, std::ratio<1, 1000>>;
	hoveranime_.Duration(timespan(400));

	return OnSetup(this);
}

LRESULT CButton::OnSize(WPARAM state, int width, int height)
{
	RefreshView();
	return 0;
}

LRESULT CButton::OnPaint()
{
	int w = rect_.width;
	int h = rect_.height;
	if (w >= 0 && h >= 0)
	{
		Redraw(w, h);
	}
	ValidateView();
	return 0;
}

LRESULT CButton::OnLbtndown(int state, int x, int y)
{
	Status(hover_activate);
	return 0;
}

LRESULT CButton::OnLbtnup(int state, int x, int y)
{
	if (hoverstat_ == hover_activate)
	{
		Status(hover_hovering);
		OnClick(this);
	}
	return 0;
}

LRESULT CButton::OnMousemove(int state, int x, int y)
{
	if (hoverstat_ == hover_inactivate)
	{
		Status(hover_hovering);
	}

	return 0;
}

LRESULT CButton::OnMouseleave()
{
	Status(hover_inactivate);
	return 0;
}

void CButton::Status(hoverstatus stat)
{
	if (hoverstat_ == stat) return;

	switch (stat)
	{
	case hover_hovering:
	{
		TRACKMOUSEEVENT tme = { sizeof(tme) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd_;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);

		hoveranime_.InsertKeyFrame(1.0f, 0.40f);
		visualb_.StartAnimation(L"Opacity", hoveranime_);
		break;
	}
	case hover_activate:
	{
		hoveranime_.InsertKeyFrame(1.0f, 0.60f);
		visualb_.StartAnimation(L"Opacity", hoveranime_);
		break;
	}
	case hover_inactivate:
	default:
	{
		hoveranime_.InsertKeyFrame(1.0f, 0.0f);
		visualb_.StartAnimation(L"Opacity", hoveranime_);
		break;
	}
	}

	hoverstat_ = stat;
}

void CButton::Redraw(LONG w, LONG h)
{
	surfacef_.Resize({ w, h });
	auto dc = BeginDraw(surfacef_);
	if (dc)
	{
		dc->Clear();

		OnRedraw(this, dc, w, h);

		EndDraw(surfacef_);
		dc->Release();
	}
}

void CButton::Bgcolor(Windows::UI::Color const& bg)
{
	visualb_.as<SpriteVisual>().Brush(
	this->Compositor().CreateColorBrush(bg));
}
