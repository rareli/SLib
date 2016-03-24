#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_WIN32)

#include "../../../inc/slib/graphics/path.h"

#include "../../../inc/slib/ui/core.h"
#include "../../../inc/slib/ui/platform.h"

#include "../../../inc/slib/core/scoped_pointer.h"

#pragma comment(lib, "gdiplus.lib")

SLIB_UI_NAMESPACE_BEGIN

class _Gdiplus_GraphicsPathInstance : public GraphicsPathInstance
{
	SLIB_DECLARE_OBJECT
public:
	Gdiplus::GraphicsPath* m_path;

public:
	_Gdiplus_GraphicsPathInstance()
	{
	}

	~_Gdiplus_GraphicsPathInstance()
	{
		delete m_path;
	}

public:
    static Ref<_Gdiplus_GraphicsPathInstance> _create(GraphicsPath* path)
	{
		Ref<_Gdiplus_GraphicsPathInstance> ret;
		ListLocker<GraphicsPathPoint> points(path->points);
		sl_size n = points.count;
		if (n > 0) {
			SLIB_SCOPED_BUFFER(Gdiplus::PointF, 1024, pts, n);
			SLIB_SCOPED_BUFFER(BYTE, 1024, types, n);
			for (sl_size i = 0; i < n; i++) {
				pts[i].X = points[i].pt.x;
				pts[i].Y = points[i].pt.y;
				types[i] = points[i].type;
			}
			points.unlock();

			Gdiplus::GraphicsPath* handle = new Gdiplus::GraphicsPath(pts, types, (INT)n, convertFillMode(path->getFillMode()));
			if (handle) {
				ret = new _Gdiplus_GraphicsPathInstance;
				if (ret.isNotNull()) {
					ret->m_path = handle;
					return ret;
				}
			}
		}
		return ret;
	}
    
    // override
	void moveTo(const Point& pt)
	{
	}
	
    // override
	void lineTo(const Point& pt)
	{
	}
	
    // override
	void cubicTo(const Point& ptControl1, const Point& ptControl2, const Point& ptEnd)
	{
	}

    // override
	void closeSubpath()
	{
	}

	static Gdiplus::FillMode convertFillMode(FillMode _mode)
	{
		Gdiplus::FillMode fillMode;
		switch (_mode) {
		case FillMode::Winding:
			fillMode = Gdiplus::FillModeWinding;
			break;
		case FillMode::Alternate:
		default:
			fillMode = Gdiplus::FillModeAlternate;
			break;
		}
		return fillMode;
	}
};

SLIB_DEFINE_OBJECT(_Gdiplus_GraphicsPathInstance, GraphicsPathInstance)

Gdiplus::GraphicsPath* UIPlatform::getGraphicsPath(GraphicsPath* path, Ref<GraphicsPathInstance>& instanceOut)
{
	if (!path) {
		return NULL;
	}
	Ref<GraphicsPathInstance> _instance = path->getInstance();
	Ref<_Gdiplus_GraphicsPathInstance> instance;
	if (_Gdiplus_GraphicsPathInstance::checkInstance(_instance.ptr)) {
		instance = Ref<_Gdiplus_GraphicsPathInstance>::from(_instance);
	} else {
		instance = _Gdiplus_GraphicsPathInstance::_create(path);
		if (instance.isNotNull()) {
			path->setInstance(instance);
		} else {
			return NULL;
		}
	}
	instanceOut = instance;
	Gdiplus::GraphicsPath* handle = instance->m_path;
	if (handle) {
		Gdiplus::FillMode fillMode = _Gdiplus_GraphicsPathInstance::convertFillMode(path->getFillMode());
		if (fillMode != handle->GetFillMode()) {
			handle->SetFillMode(fillMode);
		}
	}
	return handle;
}

Rectangle UI::getPathBounds(const Ref<GraphicsPath>& path)
{
	if (path.isNull()) {
		return Rectangle::zero();
	}
	Ref<GraphicsPathInstance> instance;
	Gdiplus::GraphicsPath* handle = UIPlatform::getGraphicsPath(path.ptr, instance);
	if (handle) {
		Rectangle ret;
		Gdiplus::RectF rc;
		handle->GetBounds(&rc);
		ret.left = (sl_real)(rc.X);
		ret.top = (sl_real)(rc.Y);
		ret.right = ret.left + (sl_real)(rc.Width);
		ret.bottom = ret.top + (sl_real)(rc.Height);
		return ret;
	}
	return Rectangle::zero();
}

sl_bool UI::checkPointInPath(const Ref<GraphicsPath>& path, const Point& _pt)
{
	if (path.isNull()) {
		return sl_false;
	}
	Ref<GraphicsPathInstance> instance;
	Gdiplus::GraphicsPath* handle = UIPlatform::getGraphicsPath(path.ptr, instance);
	if (handle) {
		return (handle->IsVisible(_pt.x, _pt.y)) != FALSE;
	}
	return sl_false;
}

SLIB_UI_NAMESPACE_END

#endif