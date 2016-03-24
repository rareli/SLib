#ifndef CHECKHEADER_SLIB_UI_VIEW_ANDROID
#define CHECKHEADER_SLIB_UI_VIEW_ANDROID

#include "../../../inc/slib/core/definition.h"

#if defined(SLIB_PLATFORM_IS_ANDROID)

#include "../../../inc/slib/ui/view.h"
#include "../../../inc/slib/ui/platform.h"

SLIB_UI_NAMESPACE_BEGIN
class Android_ViewInstance : public ViewInstance
{
public:
	Android_ViewInstance();
	~Android_ViewInstance();
	
public:
	template <class T>
	static Ref<T> create(jobject jhandle)
	{
		if (jhandle) {
			Ref<T> ret = new T();
			if (ret.isNotNull()) {
				if (ret->init(jhandle)) {
					return ret;
				}
			}
		}
		return Ref<T>::null();
	}

	template <class T>
	static Ref<T> create(View* view, ViewInstance* parent, jobject jhandle)
	{
		Ref<T> ret = create<T>(jhandle);
		if (ret.isNotNull()) {
			if (ret->applyProperties(view, parent)) {
				return ret;
			}
		}
		return Ref<T>::null();
	}

	sl_bool init(jobject jhandle);

	sl_bool applyProperties(View* view, ViewInstance* parent);

public:
	static Ref<Android_ViewInstance> getAndroidInstance(jlong jinstance);

	static Ref<View> getAndroidView(jlong jinstance);

public:
	jobject getHandle();

	jobject getContext();
	
public:
	// override
	sl_bool isValid();

	// override
	void setFocus();

	// override
	void invalidate();

	// override
	void invalidate(const Rectangle& rect);

	// override
	Rectangle getFrame();

	// override
	void setFrame(const Rectangle& frame);

	// override
	void setVisible(sl_bool flag);

	// override
	void setEnabled(sl_bool flag);

	// override
	void setOpaque(sl_bool flag);

	// override
	Point convertCoordinateFromScreenToView(const Point& ptScreen);

	// override
	Point convertCoordinateFromViewToScreen(const Point& ptView);

	// override
	void addChildInstance(const Ref<ViewInstance>& instance);

	// override
	void removeChildInstance(const Ref<ViewInstance>& instance);

protected:
	JniGlobal<jobject> m_handle;
	JniGlobal<jobject> m_context;
	
};

SLIB_JNI_BEGIN_CLASS(_JAndroidPointF, "android/graphics/PointF")
	SLIB_JNI_FLOAT_FIELD(x);
	SLIB_JNI_FLOAT_FIELD(y);
SLIB_JNI_END_CLASS

SLIB_JNI_BEGIN_CLASS(_JAndroidRectF, "android/graphics/RectF")
	SLIB_JNI_FLOAT_FIELD(left);
	SLIB_JNI_FLOAT_FIELD(top);
	SLIB_JNI_FLOAT_FIELD(right);
	SLIB_JNI_FLOAT_FIELD(bottom);
SLIB_JNI_END_CLASS

SLIB_JNI_BEGIN_CLASS(_JAndroidTouchPoint, "slib/platform/android/ui/view/UiTouchPoint")
	SLIB_JNI_FLOAT_FIELD(x);
	SLIB_JNI_FLOAT_FIELD(y);
	SLIB_JNI_FLOAT_FIELD(pressure);
SLIB_JNI_END_CLASS

SLIB_UI_NAMESPACE_END

#endif

#endif