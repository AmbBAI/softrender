#ifndef _MATH_VECTOR3_H_
#define _MATH_VECTOR3_H_

#include "base/header.h"

namespace sr
{

struct Vector3
{
	union
	{
		float f[3];
		struct { float x, y, z; };
	};

	Vector3() = default;
    Vector3(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}

    inline float Length() const;
    inline float SqrLength() const;
    inline Vector3 Normalize() const;
    inline float Dot(const Vector3& v) const;
    inline Vector3 Cross(const Vector3& v) const;

    inline Vector3 operator +() const;
    inline Vector3 operator -() const;
    inline Vector3 operator +(const Vector3& v) const;
    inline Vector3 operator -(const Vector3& v) const;
    inline Vector3 operator *(float f) const;
    inline Vector3 operator *(const Vector3& v) const;
    inline Vector3 operator /(float f) const;
    inline Vector3 operator += (const Vector3& v);
    inline Vector3 operator -= (const Vector3& v);
    inline Vector3 operator *= (float f);
    inline Vector3 operator /= (float f);

	static inline float Dot(const Vector3& a, const Vector3& b);
	static inline Vector3 Cross(const Vector3& a, const Vector3& b);
	static inline Vector3 Min(const Vector3& a, const Vector3& b);
	static inline Vector3 Max(const Vector3& a, const Vector3& b);

	static inline Vector3 LinearInterp(const Vector3& a, const Vector3& b, float t);
	static inline Vector3 TriangleInterp(const Vector3& a, const Vector3& b, const Vector3& c, float t0, float t1, float t2);

	static const Vector3 zero;
	static const Vector3 one;
	static const Vector3 front;
	static const Vector3 up;
	static const Vector3 right;
	static const Vector3 back;
	static const Vector3 down;
	static const Vector3 left;
};

} // namespace sr

#include "vector3.inl"

#endif //!_MATH_VECTOR3_H_