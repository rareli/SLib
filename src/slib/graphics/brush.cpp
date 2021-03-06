/*
 *  Copyright (c) 2008-2017 SLIBIO. All Rights Reserved.
 *
 *  This file is part of the SLib.io project.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "slib/graphics/brush.h"

#include "slib/core/safe_static.h"

namespace slib
{

	BrushDesc::BrushDesc()
	{
		style = BrushStyle::Solid;
		color = Color::White;
	}

	BrushDesc::~BrushDesc()
	{
	}

	SLIB_DEFINE_ROOT_OBJECT(Brush)

	Brush::Brush()
	{
	}

	Brush::~Brush()
	{
	}

	Ref<Brush> Brush::getDefault()
	{
		SLIB_SAFE_STATIC(Ref<Brush>, defaultBrush, create(BrushDesc()))
		if (SLIB_SAFE_STATIC_CHECK_FREED(defaultBrush)) {
			return sl_null;
		}
		return defaultBrush;
	}

	Ref<Brush> Brush::create(const BrushDesc& desc)
	{
		Ref<Brush> ret = new Brush;
		if (ret.isNotNull()) {
			ret->m_desc = desc;
		}
		return ret;
	}

	Ref<Brush> Brush::createSolidBrush(const Color& color)
	{
		BrushDesc desc;
		desc.color = color;
		return create(desc);
	}

	void Brush::getDesc(BrushDesc& desc)
	{
		desc = m_desc;
	}

	BrushStyle Brush::getStyle()
	{
		return m_desc.style;
	}

	Color Brush::getColor()
	{
		return m_desc.color;
	}

}
