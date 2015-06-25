#ifndef _BASE_VERTEX_HPP_
#define _BASE_VERTEX_HPP_

#include "base/header.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "rasterizer/clipper.hpp"

namespace rasterizer
{

struct Projection
{
	int x = 0;
	int y = 0;
	float invW = 1.0f;

	static Projection CalculateViewProjection(const Vector4& position, u32 width, u32 height)
	{
		float w = position.w;
		assert(w > 0.f);
		float invW = 1.f / w;

		Projection point;
		point.x = Mathf::RoundToInt(((position.x * invW) + 1.f) / 2.f * width);
		point.y = Mathf::RoundToInt(((position.y * invW) + 1.f) / 2.f * height);
		point.invW = invW;
		return point;
	}
};

template <typename Type>
struct Line
{
	Type v0;
	Type v1;

	Line() = default;
	Line(const Type& _v0, const Type& _v1)
		: v0(_v0), v1(_v1) {}
};

template <typename Type>
struct Triangle
{
	Type v0;
	Type v1;
	Type v2;

	Triangle() = default;
	Triangle(const Type& _v0, const Type& _v1, const Type& _v2)
		: v0(_v0), v1(_v1), v2(_v2) {}
};

}

#endif //! _BASE_CLIPPER_HPP_