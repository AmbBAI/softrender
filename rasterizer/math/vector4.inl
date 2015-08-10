#ifndef _MATH_VECTOR4_INLINE_
#define _MATH_VECTOR4_INLINE_

#include "math/mathf.h"

namespace sr
{

Vector4 Vector4::operator +() const { return *this; }
Vector4 Vector4::operator -() const { return Vector4(-x, -y, -z, -w); }
Vector4 Vector4::operator +(const Vector4& v) const { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
Vector4 Vector4::operator -(const Vector4& v) const { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
Vector4 Vector4::operator *(float f) const { return Vector4(x * f, y * f, z * f, w * f); }
Vector4 Vector4::operator *(const Vector4& v) const { return Vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
Vector4 Vector4::operator /(float f) const { return this->operator*(1.f / f); }
Vector4 Vector4::operator += (const Vector4& v) { x += v.x; y += v.y, z += v.z; w += v.w; return *this; }
Vector4 Vector4::operator -= (const Vector4& v) { x -= v.x; y -= v.y, z -= v.z; w -= v.w; return *this; }
Vector4 Vector4::operator *= (float f) { x *= f; y *= f, z *= f; w *= f; return *this; }
Vector4 Vector4::operator /= (float f) { return (*this) *= (1.f / f); }

Vector4 Vector4::LinearInterp(const Vector4& a, const Vector4& b, float t)
{
	return Vector4(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t),
		Mathf::Lerp(a.w, b.w, t));
}

Vector4 Vector4::TriangleInterp(const Vector4& a, const Vector4& b, const Vector4& c, float t0, float t1, float t2)
{
	return Vector4(
		Mathf::Terp(a.x, b.x, c.x, t0, t1, t2),
		Mathf::Terp(a.y, b.y, c.y, t0, t1, t2),
		Mathf::Terp(a.z, b.z, c.z, t0, t1, t2),
		Mathf::Terp(a.w, b.w, c.w, t0, t1, t2));
}

}

#endif // !_MATH_VECTOR4_INLINE_