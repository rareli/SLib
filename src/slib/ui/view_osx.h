/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CHECKHEADER_SLIB_UI_VIEW_OSX
#define CHECKHEADER_SLIB_UI_VIEW_OSX

#include "slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_OSX)

#include "slib/ui/view.h"
#include "slib/ui/platform.h"

namespace slib
{

	class macOS_ViewInstance : public ViewInstance
	{
	public:
		macOS_ViewInstance();
		
		~macOS_ViewInstance();
		
	public:
		static Ref<macOS_ViewInstance> create(NSView* handle, sl_bool flagFreeOnRelease);
		
		static Ref<macOS_ViewInstance> create(NSView* handle, NSView* parent, View* view);
		
	public:
		static void freeHandle(NSView* handle);
		
		NSView* getHandle();
		
	public:
		sl_bool isValid() override;
		
		void setFocus() override;
		
		void invalidate() override;
		
		void invalidate(const UIRect& rect) override;
		
		UIRect getFrame() override;
		
		void setFrame(const UIRect& frame) override;
		
		void setTransform(const Matrix3& transform) override;

		void setVisible(sl_bool flag) override;
		
		void setEnabled(sl_bool flag) override;
		
		void setOpaque(sl_bool flag) override;
		
		void setAlpha(sl_real alpha) override;
		
		void setClipping(sl_bool flag) override;
		
		void setDrawing(sl_bool flag) override;
		
		UIPointf convertCoordinateFromScreenToView(const UIPointf& ptScreen) override;
		
		UIPointf convertCoordinateFromViewToScreen(const UIPointf& ptView) override;
		
		void addChildInstance(const Ref<ViewInstance>& instance) override;
		
		void removeChildInstance(const Ref<ViewInstance>& instance) override;
		
		void bringToFront() override;
		
	public:
		void onDraw(NSRect rectDirty);
		
		sl_bool onEventKey(sl_bool flagDown, NSEvent* event);
		
		sl_bool onEventMouse(UIAction action, NSEvent* event);
		
		sl_bool onEventMouse(UIAction action, const NSPoint& point);

		sl_bool onEventMouseWheel(NSEvent* event);
		
		sl_bool onEventUpdateCursor(NSEvent* event);
		
		void applyModifiers(UIEvent* ev, NSEvent* event);
		
	private:
		void _release();
		
	protected:
		NSView* m_handle;
		sl_bool m_flagFreeOnRelease;
		
	};

	void _priv_macOS_transformViewFrame(NSPoint& origin, NSSize& size, const UIRect& frame, sl_real translationX, sl_real translationY, sl_real scaleX, sl_real scaleY, sl_real rotationRadian, sl_real anchorOffsetX, sl_real anchorOffsetY);

}

@interface _priv_Slib_macOS_ViewBase : NSView {
	@public sl_bool m_flagOpaque;
	@public sl_bool m_flagClipping;
	@public sl_bool m_flagDrawing;
}
@end

@interface _priv_Slib_macOS_ViewHandle : _priv_Slib_macOS_ViewBase {

	@public slib::WeakRef<slib::macOS_ViewInstance> m_viewInstance;

	NSTrackingArea* m_trackingArea;
}
@end


#define MACOS_VIEW_CREATE_INSTANCE_BEGIN \
	Ref<macOS_ViewInstance> ret; \
	NSView* parent = UIPlatform::getViewHandle(_parent); \
	UIRect _frame = getFrame(); \
	NSRect frame; \
	frame.origin.x = (CGFloat)(_frame.left); \
	frame.origin.y = (CGFloat)(_frame.top); \
	frame.size.width = (CGFloat)(_frame.getWidth()); \
	frame.size.height = (CGFloat)(_frame.getHeight());

#define MACOS_VIEW_CREATE_INSTANCE_END \
	if (handle != nil) { \
		Vector2 t; \
		sl_real r; \
		Vector2 s; \
		Vector2 anchor; \
		if (getFinalTranslationRotationScale(&t, &r, &s, &anchor)) { \
			NSPoint pt; \
			NSSize size; \
			_priv_macOS_transformViewFrame(pt, size, _frame, t.x, t.y, s.x, s.y, r, anchor.x, anchor.y); \
			[handle setFrameOrigin:pt]; \
			[handle setFrameSize:size]; \
			handle.frameRotation = Math::getDegreesFromRadian(r); \
		} \
		ret = macOS_ViewInstance::create(handle, parent, this); \
		if (ret.isNotNull()) { \
			handle->m_viewInstance = ret; \
		} \
	}


#endif

#endif
