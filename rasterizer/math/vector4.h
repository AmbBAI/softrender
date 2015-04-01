#ifndef _BASE_VECTOR4_H_
#define _BASE_VECTOR4_H_

#include "base/header.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Vector4
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;

	Vector4() = default;
    Vector4(float _x, float _y, float _z, float _w): x(_x), y(_y), z(_z), w(_w) {}
    Vector4(const Vector4& v): Vector4(v.x, v.y, v.z, v.w) {}
    Vector4(const Vector3& v): Vector4(v.x, v.y, v.z, 1.0f) {}

	static inline const Vector4 Lerp(const Vector4& a, const Vector4& b, float t);

	inline operator Vector3() const;
};

} // namespace rasterizer

#include "vector4.inl"

#endif //!_BASE_VECTOR4_H_