#ifndef _BASE_VECTOR4_H_
#define _BASE_VECTOR4_H_

#include "base/header.h"
#include "math/vector3.h"

namespace rasterizer
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

	inline const Vector4 Negate() const;
	inline const Vector4 Add(const Vector4& v) const;
	inline const Vector4 Subtract(const Vector4& v) const;
	inline const Vector4 Multiply(float f) const;
	inline const Vector4 Multiply(const Vector4& v) const;
	inline const Vector4 Divide(float f) const;
	inline float Dot(const Vector4& v) const;
	inline const Vector4 Cross(const Vector4& v) const;

	inline const Vector4 operator +() const;
	inline const Vector4 operator -() const;
	inline const Vector4 operator +(const Vector4& v) const;
	inline const Vector4 operator -(const Vector4& v) const;
	inline const Vector4 operator *(float f) const;
	inline const Vector4 operator *(const Vector4& v) const;
	inline const Vector4 operator /(float f) const;
	inline const Vector4 operator += (const Vector4& v);
	inline const Vector4 operator -= (const Vector4& v);
	inline const Vector4 operator *= (float f);
	inline const Vector4 operator /= (float f);

	static inline const Vector4 LinearInterp(const Vector4& a, const Vector4& b, float t);
	static inline const Vector4 TriangleInterp(const Vector4& a, const Vector4& b, const Vector4& c, float t0, float t1, float t2);

};

} // namespace rasterizer

#include "vector4.inl"

#endif //!_BASE_VECTOR4_H_