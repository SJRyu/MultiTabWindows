#include <pch.h>
#include <NativeWindows2/directx/VideoComponents.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/Win32UIThread.h>

VideoComponents::VideoComponents(Win32Window* window) :
	window_(window), indres_(window->thread_->res_.get())
{
}


VideoComponents::~VideoComponents()
{
}

void VideoComponents::CreateDxResources()
{
	InitCompositor();
	CreateSwapchain(1920, 1088);
}

void VideoComponents::InitCompositor()
{
	HR(DCompositionCreateSurfaceHandle(
		COMPOSITIONSURFACE_ALL_ACCESS, nullptr, &hcomposition_));

	compositor_ = Compositor();
	compositori_ = compositor_.as<abicomp::ICompositorInterop>();

	target_ = CreateDesktopWindowTarget(compositor_, window_->hwnd_);
	rootv_ = compositor_.CreateContainerVisual();
	rootv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	target_.Root(rootv_);
	visuals_ = rootv_.Children();

	videovisual_ = compositor_.CreateSpriteVisual();
	videovisual_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	videobrush_ = compositor_.CreateSurfaceBrush();
	videobrush_.Stretch(CompositionStretch::Fill);

	videovisual_.Brush(videobrush_);
	visuals_.InsertAtTop(videovisual_);
}

void VideoComponents::CreateSwapchain(LONG width, LONG height, DXGI_FORMAT format)
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

	//same operation but looks better
	//scsurface_ = CreateCompositionSurfaceForSwapChain(compositori_.get(), videosc_.get());
	scsurface_ = CreateCompositionSurfaceForHandle(compositori_.get(), hcomposition_);
	videobrush_.Surface(scsurface_);
}

void VideoComponents::ReleaseDxResources()
{
	ReleaseSwapchain();
}

void VideoComponents::ReleaseSwapchain()
{
	videobrush_.Surface(nullptr);
	scsurface_ = nullptr;
	backbuffer_ = nullptr;
	videosc_ = nullptr;
	SafeClose(&hcomposition_);
}

void VideoComponents::OnVideoArrive(ID3D11Texture2D* frames, UINT idx)
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
