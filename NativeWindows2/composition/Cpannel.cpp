#include <pch.h>
#include "Cpannel.h"
#include <NativeWindows2/Win32UIThread.h>

void Cpannel::Bind()
{
	vparent_ = this;
	refres_ = thread_->res_.get();

	rootv_ = refres_->compositor_.CreateContainerVisual();
	rootv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	auto target = CreateDesktopWindowTarget(refres_->compositor_, hwnd_);
	target.Root(rootv_);

	topv_ = refres_->compositor_.CreateContainerVisual();
	topv_.RelativeSizeAdjustment({ 1.0f, 1.0f });
	bottomv_ = refres_->compositor_.CreateContainerVisual();
	bottomv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	rootv_.Children().InsertAtTop(topv_);
	rootv_.Children().InsertAtBottom(bottomv_);

	visuals_ = bottomv_.Children();
}

