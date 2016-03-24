#include "../../../inc/slib/ui/generic_view.h"

SLIB_UI_NAMESPACE_BEGIN

SLIB_DEFINE_OBJECT(GenericViewWithEvent, View)

GenericViewWithEvent::GenericViewWithEvent()
{
	m_flagFocusable = sl_false;
	m_flagDown = sl_false;
	m_flagHover = sl_false;
	setClickEnabled(sl_false);
}

Ref<Cursor> GenericViewWithEvent::getCursor()
{
	return m_cursor;
}

void GenericViewWithEvent::setCursor(const Ref<Cursor> &cursor)
{
	m_cursor = cursor;
}

sl_bool GenericViewWithEvent::isFocusable()
{
	return m_flagFocusable;
}

void GenericViewWithEvent::setFocusable(sl_bool flag)
{
	m_flagFocusable = flag;
}

sl_bool GenericViewWithEvent::isDownState()
{
	return m_flagDown;
}

void GenericViewWithEvent::setDownState(sl_bool flag)
{
	m_flagDown = flag;
}

sl_bool GenericViewWithEvent::isHoverState()
{
	return m_flagHover;
}

void GenericViewWithEvent::setHoverState(sl_bool flag)
{
	m_flagHover = flag;
}

void GenericViewWithEvent::onClickView(UIEvent *ev)
{
	Ref<Runnable> runnable = getOnClick();
	if (runnable.isNotNull()) {
		runnable->run();
	}
	dispatchClick();
}

void GenericViewWithEvent::onSetCursor(UIEvent* ev)
{
	Ref<Cursor> cursor = getCursor();
	if (cursor.isNotNull()) {
		Cursor::setCurrent(cursor);
		ev->preventDefault();
	}
}

void GenericViewWithEvent::_processEvents(UIEvent* ev)
{
	if (ev->isPreventedDefault()) {
		return;
	}
	UIAction action = ev->getAction();
	if (action == UIAction::LeftButtonDown || action == UIAction::TouchBegin) {
		if (isClickEnabled()) {
			setDownState(sl_true);
			ev->preventDefault();
		}
	} else if (action == UIAction::LeftButtonUp || action == UIAction::TouchEnd || action == UIAction::TouchCancel) {
		if (isClickEnabled()) {
			if (isDownState()) {
				if (getFrameBounds().containsPoint(ev->getPoint())) {
					onClickView(ev);
				}
			}
		}
		setDownState(sl_false);
	} else if (action == UIAction::MouseEnter) {
		setHoverState(sl_true);
	} else if (action == UIAction::MouseLeave) {
		setHoverState(sl_false);
	}
}

void GenericViewWithEvent::dispatchMouseEvent(UIEvent* ev)
{
	if (!ev) {
		return;
	}
	if (! (isEnabled())) {
		return;
	}
	UIAction action = ev->getAction();
	if (m_flagFocusable) {
		if (action == UIAction::LeftButtonDown || action == UIAction::RightButtonDown || action == UIAction::MiddleButtonDown) {
			setFocus();
		}
	}
	View::dispatchMouseEvent(ev);
	_processEvents(ev);
}

void GenericViewWithEvent::dispatchTouchEvent(UIEvent* ev)
{
	if (!ev) {
		return;
	}
	if (! (isEnabled())) {
		return;
	}
	UIAction action = ev->getAction();
	if (m_flagFocusable) {
		if (action == UIAction::TouchBegin) {
			setFocus();
		}
	}
	View::dispatchTouchEvent(ev);
	_processEvents(ev);
}


SLIB_DEFINE_OBJECT(GenericViewWithDrawing, GenericViewWithEvent)

GenericViewWithDrawing::GenericViewWithDrawing()
{
	m_backgroundColor = Color::zero();
	
	m_boundShape = BoundShape::Rectangle;
	
	m_roundRectBoundShapeRadius.x = 5;
	m_roundRectBoundShapeRadius.y = 5;
	
	m_borderColor = Color::Black;
	m_borderStyle = PenStyle::Solid;
	m_borderWidth = 0;
}

Ref<Drawable> GenericViewWithDrawing::getBackground()
{
	return m_background;
}

void GenericViewWithDrawing::setBackground(const Ref<Drawable>& drawable)
{
	m_background = drawable;
	invalidate();
}

Color GenericViewWithDrawing::getBackgroundColor()
{
	return m_backgroundColor;
}

void GenericViewWithDrawing::setBackgroundColor(const Color& color)
{
	m_backgroundColor = color;
	invalidate();
}

Ref<Pen> GenericViewWithDrawing::getBorderPen()
{
	return m_penBorder;
}

void GenericViewWithDrawing::setBorderPen(const Ref<Pen>& pen)
{
	m_penBorder = pen;
	invalidate();
}

Color GenericViewWithDrawing::getBorderColor()
{
	return m_borderColor;
}

void GenericViewWithDrawing::setBorderColor(const Color& color)
{
	m_borderColor = color;
	_refreshBorderPen();
}

PenStyle GenericViewWithDrawing::getBorderStyle()
{
	return m_borderStyle;
}

void GenericViewWithDrawing::setBorderStyle(PenStyle style)
{
	m_borderStyle = style;
	_refreshBorderPen();
}

sl_real GenericViewWithDrawing::getBorderWidth()
{
	return m_borderWidth;
}

void GenericViewWithDrawing::setBorderWidth(sl_real width)
{
	m_borderWidth = width;
	_refreshBorderPen();
}

void GenericViewWithDrawing::_refreshBorderPen()
{
	sl_real width = m_borderWidth;
	Ref<Pen> pen;
	if (width > 0) {
		pen = Pen::create(m_borderStyle, m_borderWidth, m_borderColor);
	}
	setBorderPen(pen);
}

BoundShape GenericViewWithDrawing::getBoundShape()
{
	return m_boundShape;
}

void GenericViewWithDrawing::setBoundShape(BoundShape shape)
{
	m_boundShape = shape;
	invalidate();
}

const Size& GenericViewWithDrawing::getRoundRectBoundShapeRadius()
{
	return m_roundRectBoundShapeRadius;
}

void GenericViewWithDrawing::setRoundRectBoundShapeRadius(const Size& radius)
{
	m_roundRectBoundShapeRadius = radius;
	invalidate();
}

void GenericViewWithDrawing::setRoundRectBoundShapeRadius(sl_real rx, sl_real ry)
{
	setRoundRectBoundShapeRadius(Size(rx, ry));
}

void GenericViewWithDrawing::setRoundRectBoundShapeRadius(sl_real radius)
{
	setRoundRectBoundShapeRadius(Size(radius, radius));
}

Ref<GraphicsPath> GenericViewWithDrawing::getBoundShapePath()
{
	return m_boundShapePath;
}

void GenericViewWithDrawing::setBoundShapePath(const Ref<GraphicsPath>& path)
{
	m_boundShapePath = path;
	invalidate();
}

void GenericViewWithDrawing::onPreDraw(Canvas* canvas)
{
}

void GenericViewWithDrawing::onPostDraw(Canvas* canvas)
{
}

void GenericViewWithDrawing::onDrawChildren(Canvas* canvas)
{
}

void GenericViewWithDrawing::onDrawBackground(Canvas* canvas)
{
	drawBackground(canvas, m_backgroundColor, m_background);
}

void GenericViewWithDrawing::onDrawBorder(Canvas* canvas)
{
	drawBorder(canvas, m_penBorder);
}

void GenericViewWithDrawing::drawBackground(Canvas *canvas, const Color &color, const Ref<Drawable>& background)
{
	Rectangle rc(Point::zero(), getSize());
	if (color.a > 0) {
		Ref<Brush> brush = Brush::createSolidBrush(color);
		canvas->fillRectangle(rc, brush);
	}
	if (background.isNotNull()) {
		sl_real width = background->getDrawableWidth();
		sl_real height = background->getDrawableHeight();
		Rectangle rcSrc(0, 0, width, height);
		canvas->draw(rc, background, rcSrc);
	}
}

void GenericViewWithDrawing::drawBorder(Canvas* canvas, const Ref<Pen>& pen)
{
	Rectangle rc(Point::zero(), getSize());
	if (pen.isNotNull()) {
		switch (m_boundShape) {
			case BoundShape::RoundRect:
				rc.left += 1;
				rc.top += 1;
				rc.right -= 2;
				rc.bottom -= 2;
				canvas->drawRoundRect(rc, getRoundRectBoundShapeRadius(), pen);
				break;
			case BoundShape::Ellipse:
				rc.left += 1;
				rc.top += 1;
				rc.right -= 2;
				rc.bottom -= 2;
				canvas->drawEllipse(rc, pen);
				break;
			case BoundShape::Path:
				canvas->drawPath(getBoundShapePath(), pen);
				break;
			case BoundShape::Rectangle:
			default:
				rc.right -= 1;
				rc.bottom -= 1;
				canvas->setAntiAlias(sl_false);
				canvas->drawRectangle(rc, pen);
				canvas->setAntiAlias(sl_true);
				break;
		}
	}
}

void GenericViewWithDrawing::dispatchDraw(Canvas* canvas)
{
	Rectangle rc(Point::zero(), getSize());
	onPreDraw(canvas);
	{
		CanvasStatusScope scope(canvas);
		switch (m_boundShape) {
			case BoundShape::RoundRect:
				rc.right -= 1;
				rc.bottom -= 1;
				canvas->clipToRoundRect(rc, getRoundRectBoundShapeRadius());
				break;
			case BoundShape::Ellipse:
				rc.right -= 1;
				rc.bottom -= 1;
				canvas->clipToEllipse(rc);
				break;
			case BoundShape::Path:
				canvas->clipToPath(getBoundShapePath());
				break;
			case BoundShape::Rectangle:
			default:
				//canvas->clipToRectangle(rc);
				break;
		}
		onDrawBackground(canvas);
		onDraw(canvas);
		onDrawChildren(canvas);
	}
	onDrawBorder(canvas);
	onPostDraw(canvas);
}


SLIB_DEFINE_OBJECT(GenericView, GenericViewWithDrawing)

GenericView::GenericView()
{
	m_paddingLeft = 0;
	m_paddingTop = 0;
	m_paddingRight = 0;
	m_paddingBottom = 0;
}

sl_real GenericView::getPaddingLeft()
{
	return m_paddingLeft;
}

void GenericView::setPaddingLeft(sl_real paddingLeft)
{
	setPadding(paddingLeft, m_paddingTop, m_paddingRight, m_paddingBottom);
}

sl_real GenericView::getPaddingTop()
{
	return m_paddingTop;
}

void GenericView::setPaddingTop(sl_real paddingTop)
{
	setPadding(m_paddingLeft, paddingTop, m_paddingRight, m_paddingBottom);
}

sl_real GenericView::getPaddingRight()
{
	return m_paddingRight;
}

void GenericView::setPaddingRight(sl_real paddingRight)
{
	setPadding(m_paddingLeft, m_paddingTop, paddingRight, m_paddingBottom);
}

sl_real GenericView::getPaddingBottom()
{
	return m_paddingBottom;
}

void GenericView::setPaddingBottom(sl_real paddingBottom)
{
	setPadding(m_paddingLeft, m_paddingTop, m_paddingRight, paddingBottom);
}

void GenericView::setPadding(sl_real left, sl_real top, sl_real right, sl_real bottom)
{
	m_paddingLeft = left;
	m_paddingTop = top;
	m_paddingRight = right;
	m_paddingBottom = bottom;
	invalidate();
}

void GenericView::setPadding(sl_real padding)
{
	setPadding(padding, padding, padding, padding);
}

Rectangle GenericView::getContentBounds()
{
	Size size = getSize();
	Rectangle ret(m_paddingLeft, m_paddingTop, size.x - m_paddingRight, size.y - m_paddingBottom);
	return ret;
}

SLIB_UI_NAMESPACE_END