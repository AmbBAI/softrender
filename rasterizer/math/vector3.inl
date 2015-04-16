#ifndef _MATH_VECTOR3_INLINE_
#define _MATH_VECTOR3_INLINE_

#include "mathf.h"

namespace rasterizer
{

float Vector3::Length() const { return Mathf::Sqrt(SqrLength()); }
float Vector3::SqrLength() const { return (x * x + y * y + z * z); }
const Vector3 Vector3::Normalize() const { return Multiply(Mathf::InvSqrt(SqrLength())); }
const Vector3 Vector3::Negate() const { return Vector3(-x, -y, -z); }
const Vector3 Vector3::Add(const Vector3 &v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
const Vector3 Vector3::Subtract(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
const Vector3 Vector3::Multiply(float f) const { return Vector3(x * f, y * f, z * f); }
const Vector3 Vector3::Multiply(const Vector3& v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
const Vector3 Vector3::Divide(float f) const { return Multiply(1.f / f); }

const Vector3 Vector3::operator +() const { return *this; }
const Vector3 Vector3::operator -() const { return Negate(); }
const Vector3 Vector3::operator +(const Vector3& v) const { return Add(v); }
const Vector3 Vector3::operator -(const Vector3& v) const { return Subtract(v); }
const Vector3 Vector3::operator *(float f) const { return Multiply(f); }
const Vector3 Vector3::operator *(const Vector3& v) const { return Multiply(v); }
const Vector3 Vector3::operator /(float f) const { return Divide(f); }
const Vector3 Vector3::operator += (const Vector3& v) { x += v.x; y += v.y, z += v.z; return *this; }
const Vector3 Vector3::operator -= (const Vector3& v) { x -= v.x; y -= v.y, z -= v.z; return *this; }
const Vector3 Vector3::operator *= (float f) { x *= f; y *= f, z *= f; return *this; }
const Vector3 Vector3::operator /= (float f) { return (*this) *= (1.f / f); }
    
float Vector3::Dot(const Vector3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}
    
const Vector3 Vector3::Cross(const Vector3& v) const
{
    return Vector3(
                   y * v.z - z * v.y,
                   z * v.x - x * v.z,
                   x * v.y - y * v.x
                   );
}

    
const Vector3 Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
	return Vector3(
		Mathf::Lerp(a.x, b.x, t),
		Mathf::Lerp(a.y, b.y, t),
		Mathf::Lerp(a.z, b.z, t));
}

}

#endif // !_MATH_VECTOR3_INLINE_