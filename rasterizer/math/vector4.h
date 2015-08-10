#ifndef _MATH_VECTOR4_H_
#define _MATH_VECTOR4_H_

#include "base/header.h"
#include "math/vector3.h"

namespace sr
{

struct Vector4
{
	union
	{
		float f[4];
		struct  
		{
            union
            {
                struct { float x, y, z; };
                Vector3 xyz;
            };
			float w;
		};
//#if _MATH_SIMD_INTRINSIC_
//		__m128 m;
//#endif
	};

	Vector4() = default;
    Vector4(float _x, float _y, float _z, float _w): x(_x), y(_y), z(_z), w(_w) {}
	Vector4(const Vector3& _xyz, float _w) : xyz(_xyz), w(_w) {}

	inline Vector4 operator +() const;
	inline Vector4 operator -() const;
	inline Vector4 operator +(const Vector4& v) const;
	inline Vector4 operator -(const Vector4& v) const;
	inline Vector4 operator *(float f) const;
	inline Vector4 operator *(const Vector4& v) const;
	inline Vector4 operator /(float f) const;
	inline Vector4 operator += (const Vector4& v);
	inline Vector4 operator -= (const Vector4& v);
	inline Vector4 operator *= (float f);
	inline Vector4 operator /= (float f);

	static inline Vector4 LinearInterp(const Vector4& a, const Vector4& b, float t);
	static inline Vector4 TriangleInterp(const Vector4& a, const Vector4& b, const Vector4& c, float t0, float t1, float t2);

};

} // namespace rasterizer

#include "vector4.inl"

#endif //!_MATH_VECTOR4_H_