#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_IOS)

#include "../../../inc/slib/ui/label_view.h"

#include "view_ios.h"

@interface _Slib_iOS_LabelTextField : UILabel {
	
	@public slib::WeakRef<slib::iOS_ViewInstance> m_viewInstance;
	
}
@end

SLIB_UI_NAMESPACE_BEGIN
class _LabelView : public LabelView
{
public:
	void applyProperties(UILabel* handle)
	{
		[handle setText:(Apple::getNSStringFromString(m_text))];
		[handle setTextAlignment:translateAlignment(m_textAlignment)];
		if (m_flagBorder) {
			[handle.layer setBorderColor:([[UIColor grayColor] CGColor])];
			[handle.layer setBorderWidth:1];
		} else {
			[handle.layer setBorderWidth:0];
		}
		[handle setTextColor:(UIPlatform::getUIColorFromColor(m_textColor))];
		[handle setBackgroundColor:(UIPlatform::getUIColorFromColor(m_backgroundColor))];
		Ref<FontInstance> fontInstance;
		Ref<Font> font = m_font;
		UIFont* hFont = UIPlatform::getUIFont(font.ptr, fontInstance);
		if (hFont != nil) {
			[handle setFont:hFont];
		}
	}
	
	static NSTextAlignment translateAlignment(Alignments _align)
	{
		Alignments align = _align & Alignment::HorizontalMask;
		if (align == Alignment::Center) {
			return NSTextAlignmentCenter;
		} else if (align == Alignment::Right) {
			return NSTextAlignmentRight;
		}
		return NSTextAlignmentLeft;
	}
	
	static Alignments translateAlignmentReverse(NSTextAlignment align)
	{
		if (align == NSTextAlignmentCenter) {
			return Alignment::Center;
		} else if (align == NSTextAlignmentRight) {
			return Alignment::Right;
		}
		return Alignment::Left;
	}
};

Ref<ViewInstance> LabelView::createInstance(ViewInstance* _parent)
{
	IOS_VIEW_CREATE_INSTANCE_BEGIN
	_Slib_iOS_LabelTextField* handle = [[_Slib_iOS_LabelTextField alloc] initWithFrame:frame];
	if (handle != nil) {
		((_LabelView*)this)->applyProperties(handle);
	}
	IOS_VIEW_CREATE_INSTANCE_END
	return ret;
}

String LabelView::getText()
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		NSString* s = [tv text];
		m_text = Apple::getStringFromNSString(s);
	}
	return m_text;
}

void LabelView::setText(const String& value)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		[tv setText:(Apple::getNSStringFromString(value))];
	}
	m_text = value;
}

sl_bool LabelView::isBorder()
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		m_flagBorder = [tv.layer borderWidth] > 0;
	}
	return m_flagBorder;
}

void LabelView::setBorder(sl_bool flag)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		if (flag) {
			[tv.layer setBorderColor:([[UIColor grayColor] CGColor])];
			[tv.layer setBorderWidth:1];
		} else {
			[tv.layer setBorderWidth:0];
		}
	}
	m_flagBorder = flag;
}

Alignments LabelView::getTextAlignment()
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		m_textAlignment = _LabelView::translateAlignmentReverse(tv.textAlignment);
	}
	return m_textAlignment;
}

void LabelView::setTextAlignment(Alignments align)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		[tv setTextAlignment:_LabelView::translateAlignment(align)];
	}
	m_textAlignment = align;
}

Color LabelView::getTextColor()
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		m_textColor = UIPlatform::getColorFromUIColor(tv.textColor);
	}
	return m_textColor;
}

void LabelView::setTextColor(const Color& color)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		[tv setTextColor:(UIPlatform::getUIColorFromColor(color))];
	}
	m_textColor = color;
}

Color LabelView::getBackgroundColor()
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		m_backgroundColor = UIPlatform::getColorFromUIColor(tv.backgroundColor);
	}
	return m_backgroundColor;
}

void LabelView::setBackgroundColor(const Color& color)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		[tv setBackgroundColor:(UIPlatform::getUIColorFromColor(color))];
	}
	m_backgroundColor = color;
}

void LabelView::setFont(const Ref<Font>& font)
{
	UIView* handle = UIPlatform::getViewHandle(this);
	if (handle != nil && [handle isKindOfClass:[UILabel class]]) {
		UILabel* tv = (UILabel*)handle;
		Ref<FontInstance> fontInstance;
		UIFont* hFont = UIPlatform::getUIFont(font.ptr, fontInstance);
		if (hFont != nil) {
			[tv setFont:hFont];
		}
	}
	m_font = font;
}
SLIB_UI_NAMESPACE_END

@implementation _Slib_iOS_LabelTextField
-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	if (self != nil) {
		UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(labelTapped)];
		tapGestureRecognizer.numberOfTapsRequired = 1;
		[self addGestureRecognizer:tapGestureRecognizer];
		self.userInteractionEnabled = YES;
	}
	return self;
}

- (void)labelTapped
{
	slib::Ref<slib::iOS_ViewInstance> instance = m_viewInstance;
	if (instance.isNotNull()) {
		instance->onClick();
	}
}
@end

#endif