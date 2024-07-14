#include <pch.h>
#include <NativeWindows2/windows/VideoWindow.h>
#include <NativeWindows2/Win32UIThread.h>

VideoWindow::VideoWindow(WinArgs const& args) :
	D2dWindow1(args), indres_(thread_->res_.get())
{
	assert(parent_ != nullptr);
	
}

VideoWindow::~VideoWindow()
{

}

LRESULT VideoWindow::OnCreate(LPCREATESTRUCT createstr)
{
	InitCompositor1(d2dparent_);
	if (wstyle_ & WS_VISIBLE)
	{
		rootv_.IsVisible(true);
	}
	else
	{
		rootv_.IsVisible(false);
	}

	CreateVideoVisual();

	return OnCreate1(createstr);
}

void VideoWindow::OnClose()
{
	ReleaseSwapchain();

	OnClose1();
}

void VideoWindow::CreateVideoVisual()
{
	HR(DCompositionCreateSurfaceHandle(
		COMPOSITIONSURFACE_ALL_ACCESS, nullptr, &hcomposition_));

	videovisual_ = indres_->compositor_.CreateSpriteVisual();
	videovisual_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	videobrush_ = indres_->compositor_.CreateSurfaceBrush();
	videobrush_.Stretch(CompositionStretch::Fill);

	videovisual_.Brush(videobrush_);
	visuals_.InsertAtTop(videovisual_);

	CreateSwapchain(1920, 1088);
}

void VideoWindow::CreateSwapchain(
	LONG width, LONG height, DXGI_FORMAT format)
{
	scdesc_.Width = width;
	scdesc_.Height = height;
	scdesc_.Format = format;
	scdesc_.SampleDesc.Count = 1;
	scdesc_.SampleDesc.Quality = 0;
	scdesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scdesc_.BufferCount = 2;
	scdesc_.Scaling = DXGI_SCALING_STRETCH;
	scdesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scdesc_.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	scdesc_.Flags = DXGI_SWAP_CHAIN_FLAG_YUV_VIDEO |
		DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
		DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	IDXGISwapChain1* sc;
	HR(DxgiMediaFactory()->CreateSwapChainForCompositionSurfaceHandle(
		D3dDevice(),
		hcomposition_,
		&scdesc_,
		nullptr,
		&sc));
	HR(sc->QueryInterface(videosc_.put()));
	sc->Release();

	HR(videosc_->SetMaximumFrameLatency(1));
	HR(videosc_->GetBuffer(0, __uuidof(backbuffer_.get()), backbuffer_.put_void()));

	HR(videosc_->Present(0, 0));

	//same operation but looks intuitive
	//scsurface_ = CreateCompositionSurfaceForSwapChain(compositori_.get(), videosc_.get());
	scsurface_ = CreateCompositionSurfaceForHandle(indres_->compositori_.get(), hcomposition_);
	videobrush_.Surface(scsurface_);
}

void VideoWindow::ReleaseSwapchain()
{
	videobrush_.Surface(nullptr);
	scsurface_ = nullptr;
	backbuffer_ = nullptr;
	videosc_ = nullptr;
	SafeClose(&hcomposition_);
}

void VideoWindow::OnVideoArrive(ID3D11Texture2D* frames, UINT idx)
{
	D3D11_TEXTURE2D_DESC desc;
	backbuffer_->GetDesc(&desc);

	// We might be able to straight up present it without copy.
	// Windows 11 :
	// https://learn.microsoft.com/windows/win32/comp_swapchain/comp-swapchain-examples
	// Windows 10 :
	// Can we just use the surface from "IDXGIResource1::CreateSharedHandle" and bind it to visual?
	// Maybe the surface from the sharedhandle already has copy process.
	// Or Binding everytime is worse than copy, can't figure.
	// feels better move to windows 11.
	D3dc()->CopySubresourceRegion1(
		backbuffer_.get(), 0,
		0, 0, 0,
		frames, idx, &vbox_, D3D11_COPY_DISCARD);

	videosc_->Present(0, DXGI_PRESENT_ALLOW_TEARING);
}