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

	static Projection CalculateViewProjection(const Vector4& hc, u32 width, u32 height)
	{
		float w = hc.w;
		assert(w > 0.f);
		float invW = 1.f / w;

		Projection point;
		point.x = Mathf::RoundToInt(((hc.x * invW) + 1.f) / 2.f * width);
		point.y = Mathf::RoundToInt(((hc.y * invW) + 1.f) / 2.f * height);
		point.invW = invW;
		return point;
	}
};

template <typename VertexType>
struct Line
{
	VertexType v0;
	VertexType v1;

	Line() = default;
	Line(const VertexType& _v0, const VertexType& _v1) : v0(_v0), v1(_v1) {}
};

template <typename VertexType>
struct Triangle
{
	VertexType v0;
	VertexType v1;
	VertexType v2;

	Triangle() = default;
	Triangle(const VertexType& _v0, const VertexType& _v1, const VertexType& _v2) : v0(_v0), v1(_v1), v2(_v2) {}
};

}

#endif //! _BASE_CLIPPER_HPP_