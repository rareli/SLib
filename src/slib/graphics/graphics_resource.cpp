/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/graphics/resource.h"

#include "slib/graphics/util.h"
#include "slib/core/scoped.h"

namespace slib
{

	Ref<Image> _priv_ImageResourceEntry::getImage()
	{
		Ref<Image>& s = *((Ref<Image>*)((void*)&image));
		if (flag_load) {
			return s;
		}
		SpinLocker locker((SpinLock*)((void*)&lock));
		if (flag_load) {
			return s;
		}
		s = Image::loadFromMemory(source_bytes, source_size);
		flag_load = sl_true;
		return s;
	}

	Ref<Image> _priv_ImageResourceEntry::getMatchingImage(sl_uint32 reqWidth, sl_uint32 reqHeight)
	{
		if (reqWidth == 0 || reqHeight == 0) {
			return sl_null;
		}
		Ref<Image>& s = *((Ref<Image>*)((void*)&image));
		if (flag_load) {
			return s;
		}
		SpinLocker locker((SpinLock*)((void*)&lock));
		if (flag_load) {
			return s;
		}
		if (s.isNotNull()) {
			if (s->getWidth() >= reqWidth && s->getHeight() >= reqHeight) {
				return s;
			}
		}
		sl_uint32 zoomLevel = 1;
		sl_uint32 widthZoom = width;
		sl_uint32 heightZoom = height;
		while (widthZoom >= reqWidth && heightZoom >= reqHeight) {
			widthZoom >>= 1;
			heightZoom >>= 1;
			zoomLevel <<= 1;
		}
		if (zoomLevel <= 2) {
			s = Image::loadFromMemory(source_bytes, source_size);
			flag_load = sl_true;
		} else {
			Ref<Image> t = Image::loadFromMemory(source_bytes, source_size);
			if (t.isNotNull()) {
				zoomLevel >>= 1;
				s = t->scale(width / zoomLevel, height / zoomLevel);
			}
		}
		return s;
	}

	_priv_ImageResourceFreeStatic::_priv_ImageResourceFreeStatic(_priv_ImageResourceEntry* entries)
	{
		m_entries = entries;
	}

	_priv_ImageResourceFreeStatic::~_priv_ImageResourceFreeStatic()
	{
		_priv_ImageResourceEntry* entry;
		entry = m_entries;
		while (entry->flagValid) {
			(*((Ref<Image>*)((void*)&(entry->image)))).Ref<Image>::~Ref();
			entry->flagValid = sl_false;
			entry++;
		}
	}

	Ref<Image> _priv_ImageResource_getImage(_priv_ImageResourceEntry* entries, sl_uint32 requiredWidth, sl_uint32 requiredHeight)
	{
		_priv_ImageResourceEntry* entry;
		if (requiredWidth == 0 && requiredHeight == 0) {
			entry = entries;
			while (entry->flagValid) {
				return entry->getImage();
			}
			return sl_null;
		}
		
		sl_uint32 minSize = 0;
		_priv_ImageResourceEntry* minEntry = sl_null;
		entry = entries;
		while (entry->flagValid) {
			sl_uint32 width = entry->width;
			sl_uint32 height = entry->height;
			if (width >= requiredWidth && height >= requiredHeight) {
				sl_uint32 size = width * height;
				if (minSize == 0 || size < minSize) {
					minSize = size;
					minEntry = entry;
				}
			}
			entry++;
		}
		if (minEntry) {
			return minEntry->getMatchingImage(requiredWidth, requiredHeight);
		}
		sl_uint32 maxSize = 0;
		_priv_ImageResourceEntry* maxEntry = sl_null;
		entry = entries;
		while (entry->flagValid) {
			sl_uint32 width = entry->width;
			sl_uint32 height = entry->height;
			if (width < requiredWidth || height < requiredHeight) {
				sl_uint32 size = width * height;
				if (maxSize == 0 || size > maxSize) {
					maxSize = size;
					maxEntry = entry;
				}
			}
			entry++;
		}
		if (maxEntry) {
			return maxEntry->getMatchingImage(requiredWidth, requiredHeight);
		}
		return sl_null;
	}

	List< Ref<Image> > _priv_ImageResource_getImages(_priv_ImageResourceEntry* entries)
	{
		List< Ref<Image> > ret;
		_priv_ImageResourceEntry* entry;
		entry = entries;
		while (entry->flagValid) {
			Ref<Image> image = entry->getImage();
			if (image.isNotNull()) {
				ret.add_NoLock(image);
			}
			entry++;
		}
		return ret;
	}


	class _priv_ImageResource_Drawable : public Drawable
	{
		SLIB_DECLARE_OBJECT
		
	private:
		_priv_ImageResourceEntry* m_entries;
		sl_uint32 m_width;
		sl_uint32 m_height;

	public:
		_priv_ImageResource_Drawable(_priv_ImageResourceEntry* entries, sl_uint32 width, sl_uint32 height)
		{
			m_entries = entries;
			m_width = width;
			m_height = height;
		}
		
	public:
		sl_real getDrawableWidth() override
		{
			return (sl_real)m_width;
		}
		
		sl_real getDrawableHeight() override
		{
			return (sl_real)m_height;
		}
		
		void onDraw(Canvas* canvas, const Rectangle& rectDst, const Rectangle& rectSrc, const DrawParam& param) override
		{
			Rectangle rectDstWhole = GraphicsUtil::transformRectangle(rectDst, rectSrc, Rectangle(0, 0, (float)m_width, (float)m_height));
			sl_int32 width = (sl_int32)(rectDstWhole.getWidth());
			sl_int32 height = (sl_int32)(rectDstWhole.getHeight());
			if (width > 0 && height > 0) {
				Ref<Image> image = _priv_ImageResource_getImage(m_entries, width, height);
				if (image.isNotNull()) {
					float fx = (float)(image->getWidth()) / (float)(m_width);
					float fy = (float)(image->getHeight()) / (float)(m_height);
					Rectangle r;
					r.left = rectSrc.left * fx;
					r.top = rectSrc.top * fy;
					r.right = rectSrc.right * fx;
					r.bottom = rectSrc.bottom * fy;
					canvas->draw(rectDst, image, r, param);
				}
			}
		}

		void onDrawAll(Canvas* canvas, const Rectangle& rectDst, const DrawParam& param) override
		{
			sl_int32 width = (sl_int32)(rectDst.getWidth());
			sl_int32 height = (sl_int32)(rectDst.getHeight());
			if (width > 0 && height > 0) {
				Ref<Image> image = _priv_ImageResource_getImage(m_entries, width, height);
				if (image.isNotNull()) {
					canvas->draw(rectDst, image, param);
				}
			}
		}
		
	};

	SLIB_DEFINE_OBJECT(_priv_ImageResource_Drawable, Drawable)


	class _priv_ImageResource_SimpleDrawable : public Drawable
	{
		SLIB_DECLARE_OBJECT
		
	private:
		_priv_ImageResourceEntry* m_entry;
		sl_uint32 m_width;
		sl_uint32 m_height;
		
	public:
		_priv_ImageResource_SimpleDrawable(_priv_ImageResourceEntry* entry, sl_uint32 width, sl_uint32 height)
		{
			m_entry = entry;
			m_width = width;
			m_height = height;
		}
		
	public:
		sl_real getDrawableWidth() override
		{
			return (sl_real)m_width;
		}
		
		sl_real getDrawableHeight() override
		{
			return (sl_real)m_height;
		}
		
		void onDraw(Canvas* canvas, const Rectangle& rectDst, const Rectangle& rectSrc, const DrawParam& param) override
		{
			Rectangle rectDstWhole = GraphicsUtil::transformRectangle(rectDst, rectSrc, Rectangle(0, 0, (float)m_width, (float)m_height));
			sl_int32 width = (sl_int32)(rectDstWhole.getWidth());
			sl_int32 height = (sl_int32)(rectDstWhole.getHeight());
			if (width > 0 && height > 0) {
				Ref<Image> image = m_entry->getMatchingImage(width, height);
				if (image.isNotNull()) {
					float fx = (float)(image->getWidth()) / (float)(m_width);
					float fy = (float)(image->getHeight()) / (float)(m_height);
					Rectangle r;
					r.left = rectSrc.left * fx;
					r.top = rectSrc.top * fy;
					r.right = rectSrc.right * fx;
					r.bottom = rectSrc.bottom * fy;
					canvas->draw(rectDst, image, r, param);
				}
			}
		}
		
		void onDrawAll(Canvas* canvas, const Rectangle& rectDst, const DrawParam& param) override
		{
			sl_int32 width = (sl_int32)(rectDst.getWidth());
			sl_int32 height = (sl_int32)(rectDst.getHeight());
			if (width > 0 && height > 0) {
				Ref<Image> image = m_entry->getMatchingImage(width, height);
				if (image.isNotNull()) {
					canvas->draw(rectDst, image, param);
				}
			}
		}
		
	};

	SLIB_DEFINE_OBJECT(_priv_ImageResource_SimpleDrawable, Drawable)


	Ref<Drawable> _priv_ImageResource_get(_priv_ImageResourceEntry* entries, sl_uint32 width, sl_uint32 height)
	{
		if (entries->flagValid) {
			if (!(entries[1].flagValid)) {
				return new _priv_ImageResource_SimpleDrawable(entries, width, height);
			} else {
				return new _priv_ImageResource_Drawable(entries, width, height);
			}
		}
		return sl_null;
	}

}
