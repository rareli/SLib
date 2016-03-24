#include "../../../inc/slib/ui/window.h"

#include "../../../inc/slib/ui/view.h"
#include "../../../inc/slib/ui/view_group.h"
#include "../../../inc/slib/ui/core.h"

SLIB_UI_NAMESPACE_BEGIN

WindowInstanceParam::WindowInstanceParam()
{
	flagBorderless = sl_false;
	flagShowTitleBar = sl_false;
	flagFullScreen = sl_true;
	flagCenterScreen = sl_true;
	flagDialog = sl_false;
	
#if defined(SLIB_PLATFORM_IS_ANDROID)
	activity = sl_null;
#endif
}

Rectangle WindowInstanceParam::calculateRegion(const Rectangle& screenFrame) const
{
	Rectangle frame;
	if (flagFullScreen) {
		frame.setLeftTop(0, 0);
		frame.setSize(screenFrame.getSize());
	} else {
		if (flagCenterScreen) {
			frame.setLeftTop(screenFrame.getWidth() / 2 - size.x / 2, screenFrame.getHeight() / 2 - size.y / 2);
		} else {
			frame.setLeftTop(location);
		}
		frame.setSize(size);
	}
	return frame;
}

WindowParam::WindowParam()
{
	init();
	flagShowTitleBar = sl_false;
	flagFullScreen = sl_true;
}

WindowParam::WindowParam(Rectangle _rect)
{
	init();
	flagShowTitleBar = sl_false;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_false;
	location.x = _rect.left;
	location.y = _rect.top;
	size.x = _rect.getWidth();
	size.y = _rect.getHeight();
}

WindowParam::WindowParam(sl_real width, sl_real height)
{
	init();
	flagShowTitleBar = sl_false;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_true;
	size.x = width;
	size.y = height;
}

WindowParam::WindowParam(sl_real x, sl_real y, sl_real width, sl_real height)
{
	init();
	flagShowTitleBar = sl_false;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_false;
	location.x = x;
	location.y = y;
	size.x = width;
	size.y = height;
}

WindowParam::WindowParam(String _title, Rectangle _rect)
{
	init();
	flagShowTitleBar = sl_true;
	title = _title;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_false;
	location.x = _rect.left;
	location.y = _rect.top;
	size.x = _rect.getWidth();
	size.y = _rect.getHeight();
}

WindowParam::WindowParam(String _title, sl_real width, sl_real height)
{
	init();
	flagShowTitleBar = sl_true;
	title = _title;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_true;
	size.x = width;
	size.y = height;
}

WindowParam::WindowParam(String _title, sl_real x, sl_real y, sl_real width, sl_real height)
{
	init();
	flagShowTitleBar = sl_true;
	title = _title;
	flagFullScreen = sl_false;
	flagCenterScreen = sl_false;
	location.x = x;
	location.y = y;
	size.x = width;
	size.y = height;
}

void WindowParam::init()
{
	backgroundColor = Color::zero();
	
	flagVisible = sl_true;
	flagMinimized = sl_false;
	flagMaximized = sl_false;
	
	flagAlwaysOnTop = sl_false;
	flagCloseButtonEnabled = sl_true;
	flagMinimizeButtonEnabled = sl_false;
	flagMaximizeButtonEnabled = sl_false;
	flagResizable = sl_false;
	alpha = 1.0f;
	flagTransparent = sl_false;
	
	flagModal = sl_false;
}

void WindowParam::setParent(const Ref<Window>& parent)
{
	if (parent.isNotNull()) {
		this->parent = parent->getWindowInstance();
	}
}


#define CHECK_INSTANCE(instance) (instance.isNotNull() && !(instance->isClosed()))

SLIB_DEFINE_OBJECT(Window, Object)

Window::Window()
{
	SLIB_REFERABLE_CONSTRUCTOR;
	
	Ref<ViewGroup> view = new ViewGroup;
	if (view.isNotNull()) {
		view->setWindow(this);
		m_viewContent = view;
	}
}

void Window::close()
{
	ObjectLocker lock(this);
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		instance->close();
	}
	detach();
}

sl_bool Window::isClosed()
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->isClosed();
	}
	return sl_true;
}

Ref<WindowInstance> Window::getParentInstance()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getParent();
	}
	return Ref<WindowInstance>::null();
}

void Window::setParentInstance(const Ref<WindowInstance>& parent)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setParent(parent);
	}
}

Ref<Window> Window::getParent()
{
	Ref<WindowInstance> instance = getParentInstance();
	if (instance.isNotNull()) {
		return instance->getWindow();
	}
	return Ref<Window>::null();
}

void Window::setParent(const Ref<Window>& parent)
{
	if (parent.isNotNull()) {
		setParentInstance(parent->m_instance);
	}
}

Ref<ViewGroup> Window::getContentView()
{
	return m_viewContent;
}

void Window::setContentView(const Ref<ViewGroup>& view)
{
	ObjectLocker lock(this);
	
	Ref<ViewGroup> orig = m_viewContent;
	Ref<ViewInstance> instance;
	if (orig.isNotNull()) {
		instance = orig->getViewInstance();
		orig->detach();
	}
	if (view.isNotNull() && instance.isNotNull()) {
		view->attach(instance);
		view->setWindow(this);
	}
	m_viewContent = view;
}

void Window::setFocus()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setFocus();
	}
}

void Window::runModal()
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		UI::runOnUIThread(SLIB_CALLBACK_WEAKREF(Window, _runModal, this));
	}
}

void Window::_runModal()
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		instance->runModal();
	}
}

Rectangle Window::getFrame()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getFrame();
	}
	return Rectangle::zero();
}

void Window::setFrame(const Rectangle& frame)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setFrame(frame);
	}
}

void Window::setFrame(sl_real left, sl_real top, sl_real width, sl_real height)
{
	Rectangle rect;
	rect.left = left;
	rect.top = top;
	rect.setSize(width, height);
	setFrame(rect);
}

Point Window::getLocation()
{
	return getFrame().getLocation();
}

void Window::setLocation(const Point& location)
{
	Rectangle frame = getFrame();
	frame.setLocation(location);
	setFrame(frame);
}

void Window::setLocation(sl_real x, sl_real y)
{
	setLocation(Point(x, y));
}

Size Window::getSize()
{
	return getFrame().getSize();
}

void Window::setSize(const Size& size)
{
	Rectangle frame = getFrame();
	frame.setSize(size);
	setFrame(frame);
}

void Window::setSize(sl_real width, sl_real height)
{
	setSize(Size(width, height));
}

Rectangle Window::getClientFrame()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getClientFrame();
	}
	return Rectangle::zero();
}

Size Window::getClientSize()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getClientSize();
	}
	return Size::zero();
}

void Window::setClientSize(const Size& size)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setClientSize(size);
	}
}

void Window::setClientSize(sl_real width, sl_real height)
{
	setClientSize(Size(width, height));
}

String Window::getTitle()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getTitle();
	}
	return String::null();
}

void Window::setTitle(const String& title)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setTitle(title);
	}
}

Color Window::getBackgroundColor()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getBackgroundColor();
	}
	return Color::zero();
}

void Window::setBackgroundColor(const Color& color)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setBackgroundColor(color);
	}
}

sl_bool Window::isMinimized()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isMinimized();
	}
	return sl_false;
}

void Window::setMinimized(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setMinimized(flag);
	}
}

sl_bool Window::isMaximized()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isMaximized();
	}
	return sl_false;
}

void Window::setMaximized(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setMaximized(flag);
	}
}

sl_bool Window::isVisible()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isVisible();
	}
	return sl_false;
}

void Window::setVisible(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setVisible(flag);
	}
}

sl_bool Window::isAlwaysOnTop()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isAlwaysOnTop();
	}
	return sl_false;
}

void Window::setAlwaysOnTop(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setAlwaysOnTop(flag);
	}
}

sl_bool Window::isCloseButtonEnabled()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isCloseButtonEnabled();
	}
	return sl_false;
}

void Window::setCloseButtonEnabled(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setCloseButtonEnabled(flag);
	}
}

sl_bool Window::isMinimizeButtonEnabled()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isMinimizeButtonEnabled();
	}
	return sl_false;
}

void Window::setMinimizeButtonEnabled(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setMinimizeButtonEnabled(flag);
	}
}

sl_bool Window::isMaximizeButtonEnabled()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isMaximizeButtonEnabled();
	}
	return sl_false;
}

void Window::setMaximizeButtonEnabled(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setMaximizeButtonEnabled(flag);
	}
}

sl_bool Window::isResizable()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isResizable();
	}
	return sl_false;
}

void Window::setResizable(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setResizable(flag);
	}
}

sl_real Window::getAlpha()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->getAlpha();
	}
	return 1;
}

void Window::setAlpha(sl_real alpha)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setAlpha(alpha);
	}
}

sl_bool Window::isTransparent()
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		return instance->isTransparent();
	}
	return sl_false;
}

void Window::setTransparent(sl_bool flag)
{
	Ref<WindowInstance> instance = m_instance;
	if (CHECK_INSTANCE(instance)) {
		instance->setTransparent(flag);
	}
}

Point Window::convertCoordinateFromScreenToWindow(const Point& ptScreen)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromScreenToWindow(ptScreen);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromScreenToWindow(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromScreenToWindow(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromScreenToWindow(rect.getRightBottom()));
	return ret;
}

Point Window::convertCoordinateFromWindowToScreen(const Point& ptWindow)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromWindowToScreen(ptWindow);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromWindowToScreen(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromWindowToScreen(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromWindowToScreen(rect.getRightBottom()));
	return ret;
}

Point Window::convertCoordinateFromScreenToClient(const Point& ptScreen)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromScreenToClient(ptScreen);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromScreenToClient(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromScreenToClient(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromScreenToClient(rect.getRightBottom()));
	return ret;
}

Point Window::convertCoordinateFromClientToScreen(const Point& ptClient)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromClientToScreen(ptClient);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromClientToScreen(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromClientToScreen(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromClientToScreen(rect.getRightBottom()));
	return ret;
}

Point Window::convertCoordinateFromWindowToClient(const Point& ptWindow)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromWindowToClient(ptWindow);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromWindowToClient(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromWindowToClient(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromWindowToClient(rect.getRightBottom()));
	return ret;
}

Point Window::convertCoordinateFromClientToWindow(const Point& ptClient)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->convertCoordinateFromClientToWindow(ptClient);
	} else {
		return Point::zero();
	}
}

Rectangle Window::convertCoordinateFromClientToWindow(const Rectangle& rect)
{
	Rectangle ret;
	ret.setLeftTop(convertCoordinateFromClientToWindow(rect.getLeftTop()));
	ret.setRightBottom(convertCoordinateFromClientToWindow(rect.getRightBottom()));
	return ret;
}

Size Window::getWindowSizeFromClientSize(const Size& sizeClient)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->getWindowSizeFromClientSize(sizeClient);
	} else {
		return Point::zero();
	}
}

Size Window::getClientSizeFromWindowSize(const Size& sizeWindow)
{
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		return instance->getClientSizeFromWindowSize(sizeWindow);
	} else {
		return Point::zero();
	}
}

Ref<WindowInstance> Window::getWindowInstance()
{
	return m_instance;
}

void Window::attach(const Ref<WindowInstance>& instance)
{
	ObjectLocker lock(this);

	detach();
	
	if (instance.isNotNull()) {
		
		instance->setWindow(this);
		
		Ref<ViewInstance> contentViewInstance = instance->getContentView();
		if (contentViewInstance.isNotNull()) {
			Rectangle rect;
			rect.setLeftTop(Point::zero());
			rect.setSize(instance->getClientSize());
			Ref<ViewGroup> view = m_viewContent;
			if (view.isNotNull()) {
				view->attach(contentViewInstance);
				view->m_frame = rect;
			}
		}
	}
	m_instance = instance;
}

void Window::detach()
{
	ObjectLocker lock(this);
	
	Ref<View> view = m_viewContent;
	if (view.isNotNull()) {
		view->detach();
	}
	Ref<WindowInstance> instance = m_instance;
	if (instance.isNotNull()) {
		instance->setWindow(Ref<Window>::null());
	}
	m_instance.setNull();
}

sl_bool Window::createWindow(const WindowParam& param)
{
	if (! (UI::isUIThread())) {
		return sl_false;
	}
	
	Ref<WindowInstance> window = createWindowInstance(param);
	
	if (window.isNotNull()) {

		window->setParent(param.parent);
		
		window->setBackgroundColor(param.backgroundColor);
		
		window->setVisible(param.flagVisible);
		window->setMinimized(param.flagMinimized);
		window->setMaximized(param.flagMaximized);
		
		window->setAlwaysOnTop(param.flagAlwaysOnTop);
		window->setCloseButtonEnabled(param.flagCloseButtonEnabled);
		window->setMinimizeButtonEnabled(param.flagMinimizeButtonEnabled);
		window->setMaximizeButtonEnabled(param.flagMaximizeButtonEnabled);
		window->setResizable(param.flagResizable);
		window->setAlpha(param.alpha);
		window->setTransparent(param.flagTransparent);

		window->setVisible(sl_true);
		
		attach(window);
		
		onCreate();
		
#if !defined(SLIB_PLATFORM_IS_OSX)
		Size sizeOld = getSize();
		if (sizeOld.x > 0 && sizeOld.y > 0) {
			Size size = sizeOld;
			onResize(size);
			if (size != sizeOld) {
				setSize(size);
			}
		}
#endif

		if (param.flagModal) {
			_runModal();
		}
		
		return sl_true;
	}
	return sl_false;
}

void Window::create(const WindowParam& param)
{
	if (UI::isUIThread()) {
		if (createWindow(param)) {
			if (param.callbackSuccess.isNotNull()) {
				param.callbackSuccess->run();
			}
		} else {
			if (param.callbackFail.isNotNull()) {
				param.callbackFail->run();
			}
		}
	} else {
		UI::runOnUIThread(SLIB_CALLBACK_REF(Window, _createCallback, this, param));
	}
}

void Window::_createCallback(WindowParam param)
{
	if (createWindow(param)) {
		if (param.callbackSuccess.isNotNull()) {
			param.callbackSuccess->run();
		}
	} else {
		if (param.callbackFail.isNotNull()) {
			param.callbackFail->run();
		}
	}
}

void Window::addView(const Ref<View>& child)
{
	if (child.isNotNull()) {
		Ref<ViewGroup> view = m_viewContent;
		if (view.isNotNull()) {
			view->addChild(child);
		}
	}
}

void Window::removeView(const Ref<View>& child)
{
	if (child.isNotNull()) {
		Ref<ViewGroup> view = m_viewContent;
		if (view.isNotNull()) {
			view->removeChild(child);
		}
	}
}

List< Ref<View> > Window::getViews()
{
	Ref<ViewGroup> view = m_viewContent;
	if (view.isNotNull()) {
		return view->getChildren();
	}
	return List< Ref<View> >::null();
}

void Window::removeAllViews()
{
	Ref<ViewGroup> view = m_viewContent;
	if (view.isNotNull()) {
		view->removeAllChildren();
	}
}

void Window::onCreate()
{
}

sl_bool Window::onClose()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	sl_bool flagClose = sl_true;
	if (listener.isNotNull()) {
		flagClose = listener->onClose(this);
	}
	if (flagClose) {
		detach();
	}
	return flagClose;
}

void Window::onActivate()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onActivate(this);
	}
}

void Window::onDeactivate()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onDeactivate(this);
	}
}

void Window::onMove()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onMove(this);
	}
}

void Window::onResize(Size& size)
{
	_refreshSize();
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onResize(this, size);
	}
}

void Window::onMinimize()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onMinimize(this);
	}
}

void Window::onDeminimize()
{
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onDeminimize(this);
	}
}

void Window::onMaximize()
{
	_refreshSize();
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onMaximize(this);
	}
}

void Window::onDemaximize()
{
	_refreshSize();
	PtrLocker<IWindowListener> listener(getEventListener());
	if (listener.isNotNull()) {
		listener->onDemaximize(this);
	}
}

void Window::_refreshSize()
{
#if !defined(SLIB_PLATFORM_IS_WIN32)
	Ref<ViewGroup> view = m_viewContent;
	if (view.isNotNull()) {
		Rectangle rect;
		rect.setLeftTop(Point::zero());
		rect.setSize(getClientSize());
		view->setFrame(rect);
	}
#endif
}


SLIB_DEFINE_OBJECT(WindowInstance, Object)

Ref<Window> WindowInstance::getWindow()
{
	return m_window;
}

void WindowInstance::setWindow(const Ref<Window>& window)
{
	m_window = window;
}

void WindowInstance::onCreate()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onCreate();
	}
}

sl_bool WindowInstance::onClose()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		return window->onClose();
	} else {
		return sl_true;
	}
}

void WindowInstance::onActivate()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onActivate();
	}
}

void WindowInstance::onDeactivate()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onDeactivate();
	}
}

void WindowInstance::onMove()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onMove();
	}
}

void WindowInstance::onResize(Size& size)
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onResize(size);
	}
}

void WindowInstance::onMinimize()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onMinimize();
	}
}

void WindowInstance::onDeminimize()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onDeminimize();
	}
}

void WindowInstance::onMaximize()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onMaximize();
	}
}

void WindowInstance::onDemaximize()
{
	Ref<Window> window = getWindow();
	if (window.isNotNull()) {
		window->onDemaximize();
	}
}

SLIB_UI_NAMESPACE_END