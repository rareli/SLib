/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/core/definition.h"

#if defined(SLIB_UI_IS_MACOS)

#include "slib/ui/scroll_view.h"

#include "view_macos.h"

@interface _priv_Slib_macOS_ScrollView : NSScrollView {
	
	@public slib::WeakRef<slib::macOS_ViewInstance> m_viewInstance;
	
}
@end

namespace slib
{

	class _priv_ScrollView : public ScrollView
	{
	public:
		void _applyContent(NSScrollView* sv)
		{
			Ref<View> viewContent = m_viewContent;
			NSView* handle = nil;
			if (viewContent.isNotNull()) {
				Ref<ViewInstance> instance = viewContent->attachToNewInstance(Ref<ViewInstance>::null());
				if (instance.isNotNull()) {
					handle = UIPlatform::getViewHandle(instance.get());
				}
			}
			[sv setDocumentView:handle];
		}
		
		void _applyProperties(NSScrollView* handle)
		{
			[handle setBorderType:(isBorder()?NSBezelBorder:NSNoBorder)];
			Color backgroundColor = getBackgroundColor();
			if (backgroundColor.a == 0) {
				handle.drawsBackground = FALSE;
			} else {
				handle.drawsBackground = TRUE;
				handle.backgroundColor = GraphicsPlatform::getNSColorFromColor(backgroundColor);
			}
			_applyContent(handle);
		}
		
		static void _onScroll(macOS_ViewInstance* instance, NSScrollView* sv)
		{
			NSClipView* clip = [sv contentView];
			if (clip != nil) {
				NSPoint pt=[clip bounds].origin;
				Ref<View> _view = instance->getView();
				if (_priv_ScrollView* view = CastInstance<_priv_ScrollView>(_view.get())) {
					view->_onScroll_NW((sl_ui_pos)(pt.x), (sl_ui_pos)(pt.y));
				}
			}
		}
	};

	Ref<ViewInstance> ScrollView::createNativeWidget(ViewInstance* _parent)
	{
		MACOS_VIEW_CREATE_INSTANCE_BEGIN
		_priv_Slib_macOS_ScrollView* handle = [[_priv_Slib_macOS_ScrollView alloc] initWithFrame:frame];
		if (handle != nil) {
			[handle setHasVerticalScroller:(isVerticalScrolling()?YES:NO)];
			[handle setHasHorizontalScroller:(isHorizontalScrolling()?YES:NO)];
			((_priv_ScrollView*)this)->_applyProperties(handle);
		}
		MACOS_VIEW_CREATE_INSTANCE_END
		return ret;
	}

	void ScrollView::_refreshContentSize_NW()
	{
	}

	void ScrollView::_setContentView_NW(const Ref<View>& view)
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			((_priv_ScrollView*)this)->_applyContent(sv);
		}
	}

	void ScrollView::_scrollTo_NW(sl_scroll_pos x, sl_scroll_pos y, sl_bool flagAnimate)
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			NSClipView* clip = [sv contentView];
			if (clip != nil) {
				NSPoint pt;
				pt.x = (CGFloat)x;
				pt.y = (CGFloat)y;
				[clip scrollToPoint:pt];
				[sv reflectScrolledClipView:clip];
			}
		}
	}

	ScrollPoint ScrollView::_getScrollPosition_NW()
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			NSClipView* clip = [sv contentView];
			if (clip != nil) {
				NSPoint pt=[clip bounds].origin;
				ScrollPoint ret;
				ret.x = (sl_scroll_pos)(pt.x);
				ret.y = (sl_scroll_pos)(pt.y);
				return ret;
			}
		}
		return ScrollPoint::zero();
	}

	ScrollPoint ScrollView::_getScrollRange_NW()
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			NSClipView* clip = [sv contentView];
			NSView* doc = [sv documentView];
			if (clip != nil && doc != nil) {
				ScrollPoint ret;
				NSRect rcDoc = [doc frame];
				NSRect rcClip = [clip bounds];
				ret.x = (sl_scroll_pos)(NSMaxX(rcDoc) - NSWidth(rcClip));
				ret.y = (sl_scroll_pos)(NSMaxY(rcDoc) - NSHeight(rcClip));
				if (ret.x < 0) {
					ret.x = 0;
				}
				if (ret.y < 0) {
					ret.y = 0;
				}
				return ret;
			}
		}
		return ScrollPoint::zero();
	}

	void ScrollView::_setBorder_NW(sl_bool flag)
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			[sv setBorderType:(flag?NSBezelBorder:NSNoBorder)];
		}
	}

	void ScrollView::_setBackgroundColor_NW(const Color& color)
	{
		NSView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[NSScrollView class]]) {
			NSScrollView* sv = (NSScrollView*)handle;
			if (color.a == 0) {
				sv.drawsBackground = FALSE;
			} else {
				sv.drawsBackground = TRUE;
				sv.backgroundColor = GraphicsPlatform::getNSColorFromColor(color);
			}
		}
	}

}

@implementation _priv_Slib_macOS_ScrollView
-(id)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self != nil) {
		NSView *contentView = [self contentView];
		if (contentView != nil) {
			[contentView setPostsBoundsChangedNotifications:YES];
			[[NSNotificationCenter defaultCenter] addObserver:self
													 selector:@selector(boundDidChange:)
														 name:NSViewBoundsDidChangeNotification
													   object:contentView];
		}
	}
	return self;
}

- (void)boundDidChange:(NSNotification *)notification {
	slib::Ref<slib::macOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::_priv_ScrollView::_onScroll(instance.get(), self);
	}
}
@end

#endif
