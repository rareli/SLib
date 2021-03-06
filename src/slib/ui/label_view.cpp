/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/ui/label_view.h"

#include "slib/core/scoped.h"
#include "slib/graphics/util.h"

namespace slib
{
	SLIB_DEFINE_OBJECT(LabelView, View)
	
	LabelView::LabelView()
	{
		setCreatingNativeWidget(sl_false);
		setSavingCanvasState(sl_false);
		
		setUsingFont(sl_true);
		
		m_textAlignment = Alignment::Left;
		m_textColor = Color::Black;
		m_multiLineMode = MultiLineMode::Single;
		
		setPadding(1, 1, 1, 1, UIUpdateMode::Init);
	}
	
	LabelView::~LabelView()
	{
	}
	
	String LabelView::getText()
	{
		return m_text;
	}
	
	void LabelView::setText(const String& text, UIUpdateMode mode)
	{
		if (m_text == text) {
			return;
		}
		
		m_text = text;
		
		if (isNativeWidget()) {
			_setText_NW(text);
		}
		invalidateLayoutFromResizeContent(mode);
	}
	
	Color LabelView::getTextColor()
	{
		return m_textColor;
	}
	
	void LabelView::setTextColor(const Color& color, UIUpdateMode mode)
	{
		m_textColor = color;
		if (isNativeWidget()) {
			_setTextColor_NW(color);
		} else {
			if (mode == UIUpdateMode::Redraw) {
				invalidate();
			}
		}
	}
	
	Alignment LabelView::getGravity()
	{
		return m_textAlignment;
	}
	
	void LabelView::setGravity(Alignment align, UIUpdateMode mode)
	{
		m_textAlignment = align;
		if (isNativeWidget()) {
			_setTextAlignment_NW(align);
		} else {
			if (mode == UIUpdateMode::Redraw) {
				invalidate();
			}
		}
	}
	
	MultiLineMode LabelView::getMultiLineMode()
	{
		return m_multiLineMode;
	}
	
	void LabelView::setMultiLineMode(MultiLineMode multiLineMode, UIUpdateMode updateMode)
	{
		m_multiLineMode = multiLineMode;
		invalidateLayoutFromResizeContent(updateMode);
	}
	
	void LabelView::onDraw(Canvas* canvas)
	{
		m_textBox.draw(canvas, m_text, getFont(), getBoundsInnerPadding(), isWidthWrapping(), m_multiLineMode, m_textAlignment, m_textColor);
	}
	
	void LabelView::onMeasureLayout(sl_bool flagHorizontal, sl_bool flagVertical, const UIRect& currentFrame)
	{
		if (!flagVertical && !flagHorizontal) {
			return;
		}
		
		sl_ui_pos paddingWidth = getPaddingLeft() + getPaddingRight();
		m_textBox.update(m_text, getFont(), (sl_real)(currentFrame.getWidth() - paddingWidth), isWidthWrapping(), m_multiLineMode, m_textAlignment);
		if (flagHorizontal) {
			setMeasuredWidth((sl_ui_pos)(m_textBox.getContentWidth()) + paddingWidth);
		}
		if (flagVertical) {
			setMeasuredHeight((sl_ui_pos)(m_textBox.getContentHeight()) + getPaddingTop() + getPaddingBottom());
		}
		
	}
	
#if !defined(SLIB_UI_IS_MACOS) && !defined(SLIB_UI_IS_IOS) && !defined(SLIB_UI_IS_WIN32) && !defined(SLIB_UI_IS_ANDROID)
	Ref<ViewInstance> LabelView::createNativeWidget(ViewInstance* parent)
	{
		return sl_null;
	}
	
	void LabelView::_setText_NW(const String& text)
	{
	}
	
	void LabelView::_setTextColor_NW(const Color& color)
	{
	}
	
	void LabelView::_setTextAlignment_NW(Alignment align)
	{
	}
	
	void LabelView::_setFont_NW(const Ref<Font>& font)
	{
	}
	
	void LabelView::_setBorder_NW(sl_bool flag)
	{
	}
	
	void LabelView::_setBackgroundColor_NW(const Color& color)
	{
	}
#endif

}
