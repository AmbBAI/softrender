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
#if _MATH_SIMD_INTRINSIC_
		__m128 m;
#endif
	};

	Vector4() = default;
    Vector4(float _x, float _y, float _z, float _w): x(_x), y(_y), z(_z), w(_w) {}
	Vector4(const Vector3& _xyz, float _w) : xyz(_xyz), w(_w) {}

	static inline const Vector4 Lerp(const Vector4& a, const Vector4& b, float t);
};

} // namespace rasterizer

#include "vector4.inl"

#endif //!_BASE_VECTOR4_H_