#include <pch.h>
#include <NativeWindows2/composition/Cvobj.h>
#include <NativeWindows2/composition/Cpannel.h>
#include <NativeWindows2/windows/Win32Window.h>
#include <NativeWindows2/Win32UIThread.h>

void Cvobj1::Bind(Cvi* parent)
{
	Cvi::Bind(parent, this);

	refres_ = parent->refres_;
	compositor_ = parent->compositor_;

	CRECT<FLOAT> rect = vrect_;
	rootv_ = refres_->compositor_.CreateContainerVisual();
	rootv_.Offset({ rect.left, rect.top, 0 });
	rootv_.Size({ rect.width, rect.height });

	auto clip = refres_->compositor_.CreateInsetClip(
		0, 0, 0, 0);
	rootv_.Clip(clip);

	parentv_ = parent->topv_.Children();
	parentv_.InsertAtTop(rootv_);

	topv_ = refres_->compositor_.CreateContainerVisual();
	topv_.RelativeSizeAdjustment({ 1.0f, 1.0f });
	bottomv_ = refres_->compositor_.CreateContainerVisual();
	bottomv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	rootv_.Children().InsertAtTop(topv_);
	rootv_.Children().InsertAtBottom(bottomv_);

	visuals_ = bottomv_.Children();

	using timespan = std::chrono::duration<int, std::ratio<1, 1000>>;
	animemove_ = refres_->compositor_.CreateVector3KeyFrameAnimation();
	animemove_.Duration(timespan(400));

	animesize_ = refres_->compositor_.CreateVector2KeyFrameAnimation();
	animesize_.Duration(timespan(400));
}

void CvRoot::Bind(Win32Window* win)
{
	vparent_ = this;
	refres_ = win->thread_->res_.get();

	rootv_ = refres_->compositor_.CreateContainerVisual();
	rootv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	auto target = CreateDesktopWindowTarget(refres_->compositor_, win->hwnd_);
	target.Root(rootv_);

	topv_ = refres_->compositor_.CreateContainerVisual();
	topv_.RelativeSizeAdjustment({ 1.0f, 1.0f });
	bottomv_ = refres_->compositor_.CreateContainerVisual();
	bottomv_.RelativeSizeAdjustment({ 1.0f, 1.0f });

	rootv_.Children().InsertAtTop(topv_);
	rootv_.Children().InsertAtBottom(bottomv_);

	visuals_ = bottomv_.Children();
}

Cvi* Cvi::HitTest(UINT msg, WPARAM wp, LPARAM lp, std::list<Cvi*>& chlist)
{
	Cvi* ret = nullptr;

	for (auto ch : chlist)
	{
		ret = ch->HitTest(msg, wp, lp);
		if (ret != nullptr)
		{
			break;
		}
	}

	return ret;
}

Cvi* CvRoot::HitTest(UINT msg, WPARAM wp, LPARAM lp)
{
	Cvi* ret = nullptr;

	if (mfocused_)
	{
		auto target = mfocused_;
		auto chlist = target->vchild_;
		while (true)
		{
			ret = Cvi::HitTest(msg, wp, lp, chlist);
			if (ret)
			{
				break;
			}
			else
			{
				auto parent = target->vparent_;
				if (parent != this)
				{
					chlist = parent->vchild_;
					chlist.remove(target);
					target = static_cast<decltype(target)>(parent);
				}
				else
				{
					break;
				}
			}
		} 
	}
	else
	{
		for (auto child : vchild_)
		{
			ret = child->HitTest(msg, wp, lp);
			if (ret)
			{
				break;
			}
		}
	}

	if (ret != mfocused_)
	{
		if (mfocused_ != nullptr)
		{
			mfocused_->OnMouseLeave(mfocused_);
		}
		mfocused_ = ret;
	}
	
	return ret;
}

Cvi* Cvobj1::HitTest(UINT msg, WPARAM wp, LPARAM lp)
{
	Cvi* ret = nullptr;
	POINT xy = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };

	bool inrect = ::PtInRect(&vrect_, xy);
	if (inrect)
	{
		ret = Cvi::HitTest(msg, wp, lp, vchild_);
		if (ret == nullptr)
		{
			if (OnHitTest(this, xy.x, xy.y))
			{
				ret = this;
			}
		}
	}
	return ret;
}

