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

#if defined(SLIB_UI_IS_IOS)

#include "slib/ui/select_view.h"

#include "view_ios.h"

@interface _priv_Slib_iOS_SelectView : UITextField<UIPickerViewDelegate, UIPickerViewDataSource, UITextFieldDelegate>
{
	@public UIPickerView* m_picker;
	
	@public sl_uint32 m_selectionBefore;
	
	@public slib::WeakRef<slib::iOS_ViewInstance> m_viewInstance;
}
@end

namespace slib
{
	class _priv_SelectView : public SelectView
	{
	public:
		sl_uint32 _getItemsCount()
		{
			return (sl_uint32)(m_titles.getCount());
		}
		
		NSString* _getItemTitle(sl_uint32 row)
		{
			String s = m_titles.getValueAt(row);
			return Apple::getNSStringFromString(s);
		}
		
		void _onSelectItem(_priv_Slib_iOS_SelectView* v, sl_uint32 row)
		{
			m_indexSelected = row;
			v.text = _getItemTitle(row);
			dispatchSelectItem(row);
		}
		
		void _onStartSelection(_priv_Slib_iOS_SelectView* v)
		{
			sl_uint32 n = m_indexSelected;
			v->m_selectionBefore = n;
			dispatch_async(dispatch_get_main_queue(), ^{
				[v->m_picker selectRow:n inComponent:0 animated:NO];
			});
		}
		
		void _onCancelSelection(_priv_Slib_iOS_SelectView* v)
		{
			_onSelectItem(v, v->m_selectionBefore);
		}
		
		void _selectItem(_priv_Slib_iOS_SelectView* v, sl_uint32 row)
		{
			v.text = _getItemTitle(row);
			[v->m_picker selectRow:row inComponent:0 animated:NO];
		}
	};
	
	Ref<ViewInstance> SelectView::createNativeWidget(ViewInstance* _parent)
	{
		IOS_VIEW_CREATE_INSTANCE_BEGIN
		_priv_Slib_iOS_SelectView* handle = [[_priv_Slib_iOS_SelectView alloc] initWithFrame:frame];
		if (handle != nil) {
			((_priv_SelectView*)this)->_selectItem(handle, m_indexSelected);
			
			Ref<Font> font = getFont();
			UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
			if (hFont != nil) {
				[handle setFont:hFont];
			}
		}
		IOS_VIEW_CREATE_INSTANCE_END
		return ret;
	}
	
	void SelectView::_getSelectedIndex_NW()
	{
	}
	
	void SelectView::_select_NW(sl_uint32 index)
	{
		if (![NSThread isMainThread]) {
			dispatch_async(dispatch_get_main_queue(), ^{
				_select_NW(index);
			});
			return;
		}
		
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[_priv_Slib_iOS_SelectView class]]) {
			_priv_Slib_iOS_SelectView* v = (_priv_Slib_iOS_SelectView*)handle;
			((_priv_SelectView*)this)->_selectItem(v, index);
		}
	}
	
	void SelectView::_refreshItemsCount_NW()
	{
		if (![NSThread isMainThread]) {
			dispatch_async(dispatch_get_main_queue(), ^{
				_refreshItemsCount_NW();
			});
			return;
		}
		
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[_priv_Slib_iOS_SelectView class]]) {
			_priv_Slib_iOS_SelectView* v = (_priv_Slib_iOS_SelectView*)handle;
			[v->m_picker reloadAllComponents];
		}
	}
	
	void SelectView::_refreshItemsContent_NW()
	{
		if (![NSThread isMainThread]) {
			dispatch_async(dispatch_get_main_queue(), ^{
				_refreshItemsContent_NW();
			});
			return;
		}
		
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[_priv_Slib_iOS_SelectView class]]) {
			_priv_Slib_iOS_SelectView* v = (_priv_Slib_iOS_SelectView*)handle;
			[v->m_picker reloadAllComponents];
		}
	}
	
	void SelectView::_setItemTitle_NW(sl_uint32 index, const String& title)
	{
		if (![NSThread isMainThread]) {
			String _title = title;
			dispatch_async(dispatch_get_main_queue(), ^{
				_setItemTitle_NW(index, _title);
			});
			return;
		}
		
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[_priv_Slib_iOS_SelectView class]]) {
			_priv_Slib_iOS_SelectView* v = (_priv_Slib_iOS_SelectView*)handle;
			[v->m_picker reloadAllComponents];
		}
	}
	
	void SelectView::_setFont_NW(const Ref<Font>& font)
	{
		if (![NSThread isMainThread]) {
			Ref<Font> _font = font;
			dispatch_async(dispatch_get_main_queue(), ^{
				_setFont_NW(_font);
			});
			return;
		}
		
		UIView* handle = UIPlatform::getViewHandle(this);
		if (handle != nil && [handle isKindOfClass:[_priv_Slib_iOS_SelectView class]]) {
			_priv_Slib_iOS_SelectView* v = (_priv_Slib_iOS_SelectView*)handle;
			UIFont* hFont = GraphicsPlatform::getUIFont(font.get(), UIPlatform::getGlobalScaleFactor());
			if (hFont != nil) {
				[v setFont:hFont];
			}
		}
	}
}

#define DROP_ICON_WIDTH 20
#define DROP_ICON_HEIGHT 12

@interface _priv_Slib_iOS_SelectView_DropIcon : UIView
{
	@public __weak _priv_Slib_iOS_SelectView* parent;
}
@end

@implementation _priv_Slib_iOS_SelectView_DropIcon
- (id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	if (self != nil) {
		[self setClipsToBounds:YES];
		[self setOpaque:FALSE];
	}
	return self;
}

- (void)drawRect:(CGRect)dirtyRect
{
	CGContextRef context = UIGraphicsGetCurrentContext();
	CGContextBeginPath(context);
	float pl = DROP_ICON_WIDTH * 0.2f;
	float pr = DROP_ICON_WIDTH * 0.3f;
	float ph = DROP_ICON_HEIGHT * 0.2f;
	CGContextSetAllowsAntialiasing(context, YES);
	CGContextSetShouldAntialias(context, YES);
	CGContextMoveToPoint(context, pl, ph);
	CGContextAddLineToPoint(context, pl + (DROP_ICON_WIDTH - pl - pr) / 2, DROP_ICON_HEIGHT - ph);
	CGContextAddLineToPoint(context, DROP_ICON_WIDTH - pr, ph);
	CGContextSetLineWidth(context, 2);
	CGContextSetLineCap(context, kCGLineCapRound);
	CGContextSetLineJoin(context, kCGLineJoinRound);
	CGContextSetRGBStrokeColor(context, 0, 0, 0, 1);
	CGContextStrokePath(context);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)theEvent
{
	[super touchesBegan:touches withEvent:theEvent];
	[parent becomeFirstResponder];
}
@end

@implementation _priv_Slib_iOS_SelectView
-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	
	if (self != nil) {
	
		self->m_selectionBefore = 0;
		
		[self setDelegate:self];
		[self setBackgroundColor:[UIColor whiteColor]];
		[self setBorderStyle:UITextBorderStyleRoundedRect];
		
		// hide the caret and its blinking
		[[self valueForKey:@"textInputTraits"] setValue:[UIColor clearColor] forKey:@"insertionPointColor"];
		
		// add icon
		_priv_Slib_iOS_SelectView_DropIcon* icon = [[_priv_Slib_iOS_SelectView_DropIcon alloc] initWithFrame:(CGRectMake(0, 0, DROP_ICON_WIDTH, DROP_ICON_HEIGHT))];
		icon->parent = self;
		self.rightView =  icon;
		self.rightViewMode = UITextFieldViewModeAlways;
		
		// picker
		UIPickerView* picker = [[UIPickerView alloc] init];
		picker.showsSelectionIndicator = YES;
		picker.dataSource = self;
		picker.delegate = self;
		self->m_picker = picker;
	
		// toolbar
		UIToolbar* toolbar = [[UIToolbar alloc] init];
		toolbar.barStyle = UIBarStyleDefault;
		[toolbar sizeToFit];
		UIBarButtonItem *flexibleSpace = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
		UIBarButtonItem* doneButton = [[UIBarButtonItem alloc] initWithTitle:@"Done" style:UIBarButtonItemStyleDone target:self action:@selector(doneClicked:)];
		UIBarButtonItem* cancelButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(cancelClicked:)];
		[toolbar setItems:[NSArray arrayWithObjects:cancelButton, flexibleSpace, doneButton, nil]];

		self.inputView = picker;
		self.inputAccessoryView = toolbar;
	}
	return self;
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
{
	return 1;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			_view->_onSelectItem(self, (sl_uint32)row);
		}
	}
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			return (NSInteger)(_view->_getItemsCount());
		}
	}
	return 0;
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component;
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			return _view->_getItemTitle((sl_uint32)row);
		}
	}
	return @"";
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)aTextField
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			if ((NSInteger)(_view->_getItemsCount()) > 0) {
				return YES;
			}
		}
	}
	return NO;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
	[self sendActionsForControlEvents:UIControlEventEditingDidBegin];
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			_view->_onStartSelection(self);
		}
	}
}

- (void)textFieldDidEndEditing:(UITextField *)aTextField {
	aTextField.userInteractionEnabled = YES;
	[self sendActionsForControlEvents:UIControlEventEditingDidEnd];
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string
{
	return NO;
}

-(void)doneClicked:(id) sender
{
	[self resignFirstResponder];
	[self sendActionsForControlEvents:UIControlEventValueChanged];
}

-(void)cancelClicked:(id)sender
{
	[self resignFirstResponder];
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		slib::Ref<slib::View> view = instance->getView();
		if (slib::_priv_SelectView* _view = slib::CastInstance<slib::_priv_SelectView>(view.get())) {
			_view->_onCancelSelection(self);
		}
	}
}
@end

#endif
