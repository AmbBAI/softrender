#ifndef _BASE_VECTOR3_H_
#define _BASE_VECTOR3_H_

#include "base/header.h"
#include "math/mathf.h"

namespace rasterizer
{

struct Vector3
{
	union
	{
		float f[3];
		struct
		{
			float x;
			float y;
			float z;
		};
//#if _MATH_SIMD_INTRINSIC_
//		__m128 m;
//#endif
	};

	Vector3() = default;
	Vector3(float _x, float _y, float _z);

	float Length() const { return Mathf::Sqrt(SqrLength()); }
	float SqrLength() const { return (x * x + y * y + z * z); }
	const Vector3 Normalize() const { return (*this) * Mathf::InvSqrt(SqrLength()); }
	const Vector3 Negate() const { return Vector3(-x, -y, -z); }
	const Vector3 Add(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	const Vector3 Subtract(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	const Vector3 Multiply(float f) const { return Vector3(x * f, y * f, z * f); }
	const Vector3 Divide(float f) const { return Multiply(1.f / f); }
	float Dot(const Vector3& v) const;
	const Vector3 Cross(const Vector3& v) const;

	const Vector3 operator +() const { return *this; }
	const Vector3 operator -() const { return Negate(); }
	const Vector3 operator +(const Vector3& v) const { return Add(v); }
	const Vector3 operator -(const Vector3& v) const { return Subtract(v); }
	const Vector3 operator *(float f) const { return Multiply(f); }
	const Vector3 operator /(float f) const { return Divide(f); }
	const Vector3 operator += (const Vector3& v) { x += v.x; y += v.y, z += v.z; return *this; }
	const Vector3 operator -= (const Vector3& v) { x -= v.x; y -= v.y, z -= v.z; return *this; }
	const Vector3 operator *= (float f) { x *= f; y *= f, z *= f; return *this; }
	const Vector3 operator /= (float f) { return (*this) * (1.f / f); }

	std::string ToString() const;
	
	static const Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 front;
	static const Vector3 up;
	static const Vector3 right;
	static const Vector3 back;
	static const Vector3 down;
	static const Vector3 left;
};

}

#endif //!_BASE_VECTOR3_H_