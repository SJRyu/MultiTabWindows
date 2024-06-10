#pragma once

namespace NativeWindows
{
	class NATIVEWINDOWS2_API InputSync
	{
	public:

		std::function<bool(void*, int, int)> OnHitTest =
			[](void* s, int x, int y) { return false; };

		std::function<void(void*)> OnMouseLeave = [](void* s) {};

		std::function<void(void*, int, int, int)> OnMouseMove = 
			[](void*, int state, int x, int y) {};

		std::function<bool(void*, WPARAM wp, LPARAM lp)> OnMouseWheel =
			[](void*, WPARAM, LPARAM)->bool { return false; };

		std::function <void(void*, int, int, int)> OnLBtnDown =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnLBtnUp =
			[](void*, int state, int x, int y) {};
		
		std::function <void(void*, int, int, int)> OnLBtnDouble =
			[](void*, int state, int x, int y) {};
		
		std::function <void(void*, int, int, int)> OnRBtnDown =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnRBtnUp =
			[](void*, int state, int x, int y) {};
		
		std::function <void(void*, int, int, int)> OnRBtnDouble =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnMBtnDown =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnMBtnUp =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnXBtnDown =
			[](void*, int state, int x, int y) {};

		std::function <void(void*, int, int, int)> OnXBtnUp =
			[](void*, int state, int x, int y) {};

	protected:

	};
}
