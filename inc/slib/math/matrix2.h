#ifndef CHECKHEADER_SLIB_MATH_MATRIX2
#define CHECKHEADER_SLIB_MATH_MATRIX2

#include "definition.h"

#include "vector2.h"

#include "../core/interpolation.h"

SLIB_MATH_NAMESPACE_BEGIN

#define SLIB_MATH_MATRIX_DETERMINANT2(m00,m01,m10,m11) ((m00)*(m11)-(m01)*(m10))

template <class T>
class SLIB_EXPORT Matrix2T
{
public:
	T m00; T m01;
	T m10; T m11;

public:
	SLIB_INLINE Matrix2T() = default;
	
	SLIB_CONSTEXPR Matrix2T(const Matrix2T<T>& other):
	 m00(other.m00), m01(other.m01),
	 m10(other.m10), m11(other.m11)
	{}

	template <class O>
	SLIB_CONSTEXPR Matrix2T(const Matrix2T<O>& other):
	 m00((T)(other.m00)), m01((T)(other.m01)),
	 m10((T)(other.m10)), m11((T)(other.m11))
	{}

	SLIB_CONSTEXPR Matrix2T(
		T _m00, T _m01,
		T _m10, T _m11
	):
	 m00(_m00), m01(_m01),
	 m10(_m10), m11(_m11)
	{}
	
public:
	static const Matrix2T<T>& zero();

	static const Matrix2T<T>& one();

	static const Matrix2T<T>& identity();
	
	static const Matrix2T<T>& fromArray(const T arr[4]);
	
	static Matrix2T<T>& fromArray(T arr[4]);

	Vector2T<T> getRow0() const;
	
	void setRow0(const Vector2T<T>& v);
	
	Vector2T<T> getRow1() const;
	
	void setRow1(const Vector2T<T>& v);

	Vector2T<T> getRow(sl_uint32 index) const;
	
	void setRow(sl_uint32 index, const Vector2T<T>& v);
	
	Vector2T<T> getColumn0() const;
	
	void setColumn0(const Vector2T<T>& v);
	
	Vector2T<T> getColumn1() const;
	
	void setColumn1(const Vector2T<T>& v);

	Vector2T<T> getColumn(sl_uint32 index) const;
	
	void setColumn(sl_uint32 index, const Vector2T<T>& v);

	T getElement(sl_uint32 row, sl_uint32 column) const;
	
	void setElement(sl_uint32 row, sl_uint32 column, const T& v);
	
	
	void add(const Matrix2T<T>& other);
	
	void subtract(const Matrix2T<T>& other);
	
	void multiply(T value);
	
	void divide(T value);
	
	Vector2T<T> multiplyLeft(const Vector2T<T>& v) const;
	
	Vector2T<T> multiplyRight(const Vector2T<T>& v) const;
	
	void multiply(const Matrix2T<T>& m);
	
	T getDeterminant() const;
	
	void makeInverse();
	
	Matrix2T<T> inverse() const;
	
	void makeTranspose();
	
	Matrix2T<T> transpose() const;
	
	void makeInverseTranspose();
	
	Matrix2T<T> inverseTranspose() const;

	Matrix2T<T> lerp(const Matrix2T<T>& target, float factor) const;
	
public:
	Matrix2T<T>& operator=(const Matrix2T<T>& other) = default;
	
	template <class O>
	Matrix2T<T>& operator=(const Matrix2T<O>& other);

	Matrix2T<T> operator+(const Matrix2T<T>& other) const;
	
	Matrix2T<T>& operator+=(const Matrix2T<T>& other);
	
	Matrix2T<T> operator-(const Matrix2T<T>& other) const;
	
	Matrix2T<T>& operator-=(const Matrix2T<T>& other);
	
	Matrix2T<T> operator-() const;

	Matrix2T<T> operator*(T value) const;
	
	Matrix2T<T>& operator*=(T value);
	
	Matrix2T<T> operator/(T value) const;
	
	Matrix2T<T>& operator/=(T value);

	Vector2T<T> operator*(const Vector2T<T>& v) const;
	
	Matrix2T<T> operator*(const Matrix2T<T>& other) const;
	
	Matrix2T<T>& operator*=(const Matrix2T<T>& other);
	
	sl_bool operator==(const Matrix2T<T>& other) const;
	
	sl_bool operator!=(const Matrix2T<T>& other) const;
	
private:
	static T _zero[4];
	static T _one[4];
	static T _identity[4];

};

SLIB_DECLARE_GEOMETRY_TYPE(Matrix2)


template <class T>
Matrix2T<T> operator*(T value, const Matrix2T<T>& m);

template <class T>
Vector2T<T> operator*(const Vector2T<T>& v, const Matrix2T<T>& m);

template <class T>
class Interpolation< Matrix2T<T> >
{
public:
	static Matrix2T<T> interpolate(const Matrix2T<T>& a, const Matrix2T<T>& b, float factor);
};

SLIB_MATH_NAMESPACE_END


SLIB_MATH_NAMESPACE_BEGIN

template <class T>
SLIB_INLINE const Matrix2T<T>& Matrix2T<T>::zero()
{
	return *(reinterpret_cast<Matrix2T<T> const*>(&_zero));
}

template <class T>
SLIB_INLINE const Matrix2T<T>& Matrix2T<T>::one()
{
	return *(reinterpret_cast<Matrix2T<T> const*>(&_one));
}

template <class T>
SLIB_INLINE const Matrix2T<T>& Matrix2T<T>::identity()
{
	return *(reinterpret_cast<Matrix2T<T> const*>(&_identity));
}

template <class T>
SLIB_INLINE const Matrix2T<T>& Matrix2T<T>::fromArray(const T arr[4])
{
	return *(reinterpret_cast<Matrix2T<T> const*>(arr));
}

template <class T>
SLIB_INLINE Matrix2T<T>& Matrix2T<T>::fromArray(T arr[4])
{
	return *(reinterpret_cast<Matrix2T<T>*>(arr));
}

template <class T>
template <class O>
SLIB_INLINE Matrix2T<T>& Matrix2T<T>::operator=(const Matrix2T<O>& other)
{
	m00 = (T)(other.m00); m01 = (T)(other.m01);
	m10 = (T)(other.m10); m11 = (T)(other.m11);
	return *this;
}

template <class T>
Matrix2T<T> operator*(T value, const Matrix2T<T>& m)
{
	Matrix2T<T> ret(m);
	ret.multiply(value);
	return ret;
}

template <class T>
Vector2T<T> operator*(const Vector2T<T>& v, const Matrix2T<T>& m)
{
	return m.multiplyLeft(v);
}


template <class T>
SLIB_INLINE Matrix2T<T> Interpolation< Matrix2T<T> >::interpolate(const Matrix2T<T>& a, const Matrix2T<T>& b, float factor)
{
	return a.lerp(b, factor);
}

SLIB_MATH_NAMESPACE_END

#endif
