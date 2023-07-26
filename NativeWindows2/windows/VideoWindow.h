#pragma once

#include <NativeWindows2/windows/D2dWindow.h>
#include <NativeWindows2/directx/IndependentRes.h>

namespace NativeWindows
{
	class NATIVEWINDOWS2_API VideoWindow : public D2dWindow1
	{
	public:

		VideoWindow(D2dWinArgs const& args);
		virtual ~VideoWindow();

		inline auto D3dDevice()
		{
			return indres_->d3dDevice_.get();
		}

		inline auto D3dc()
		{
			return indres_->d3dDc_.get();
		}

		inline auto DxgiFactory()
		{
			return indres_->dxgiFactory_.get();
		}

		inline auto DxgiMediaFactory()
		{
			return indres_->dxgiMediaFactory_.get();
		}

		inline auto Swapchain()
		{
			return videosc_.get();
		}

		inline void Present(UINT synci = 0, UINT flags = 0)
		{
			HR(videosc_->Present(synci, flags));
		}

		inline void Present1(
			DXGI_PRESENT_PARAMETERS* prm, UINT synci = 0, UINT flags = 0)
		{
			HR(videosc_->Present1(synci, flags, prm));
		}

		inline void ResizeVideo(LONG width, LONG height, DXGI_FORMAT format)
		{
			vwidth_ = std::max(2, (int)width);
			vheight_ = std::max(2, (int)height);

			vbox_.right = vwidth_;
			vbox_.bottom = vheight_;
			vbox_.back = 1;

			backbuffer_ = nullptr;
			HR(videosc_->ResizeBuffers(0, vwidth_, vheight_, format, scdesc_.Flags));
			HR(videosc_->GetBuffer(0, __uuidof(backbuffer_.get()), backbuffer_.put_void()));
		}

		void OnVideoArrive(ID3D11Texture2D* frames, UINT idx);
		
	protected:

		IndependentRes* indres_ = nullptr;

		HANDLE hcomposition_ = nullptr;
		SpriteVisual videovisual_{ nullptr };
		CompositionSurfaceBrush videobrush_{ nullptr };

		//for swapchain
		com_ptr<IDXGISwapChain4> videosc_;
		com_ptr<ID3D11Texture2D> backbuffer_;
		ICompositionSurface scsurface_{};

		enum COMPOSITIONSURFACE_ACCESS
		{
			COMPOSITIONSURFACE_READ = 1,
			COMPOSITIONSURFACE_WRITE,
			COMPOSITIONSURFACE_ALL_ACCESS,
		};
		DXGI_SWAP_CHAIN_DESC1 scdesc_{};

		void CreateVideoVisual();
		void CreateSwapchain(LONG width, LONG height, DXGI_FORMAT format = DXGI_FORMAT_NV12);
		void ReleaseSwapchain();

		virtual LRESULT CALLBACK OnCreate(LPCREATESTRUCT createstr) override;
		virtual void CALLBACK OnClose() override;
		
	private:
		UINT vwidth_;
		UINT vheight_;
		D3D11_BOX vbox_;
	};
}