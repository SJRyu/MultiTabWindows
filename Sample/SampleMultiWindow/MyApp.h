#pragma once

#include <NativeWindows2/MainLoop.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <DiscoveryThread.h>

namespace NativeWindows
{
	class ContainerWindow;
	class DiscoveryThread;

	class MyApp : public MainLoop
	{
	public:

		virtual ~MyApp();

		static inline MyApp* GetInstance()
		{
			if (instance_ == nullptr)
			{
				instance_ = new MyApp();
			}
			return (MyApp*)instance_;
		}

		inline void GetResolvedEndpoints(
			std::list<OnvifDiscoveryCpp::EndpointMetadata>* outref)
		{
			return discoveryThread_->GetResolvedEndpoints(outref);
		}

		static HMENU hmenu_;

		static inline int tabcount_ = 0;
		static inline int UpCountTab()
		{
			return (++tabcount_);
		}

	protected:

		MyApp();
		
		DiscoveryThread* discoveryThread_ = nullptr;

		virtual void CALLBACK OnAppStart() override;
		virtual void CALLBACK OnAppEnd() override;
		virtual void CALLBACK OnNewContainer(ContainerWindow* win) override;
	};
}
