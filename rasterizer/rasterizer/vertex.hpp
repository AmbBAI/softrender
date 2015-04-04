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
	float invZ = 1.0f;
	float invW = 1.0f;

	static Projection CalculateViewProjection(const Vector4& position, u32 width, u32 height)
	{
		float w = position.w;
		assert(w > 0.f);
		float invW = 1.f / w;

		Projection point;
		point.x = Mathf::RoundToInt(((position.x * invW) + 1.f) / 2.f * width);
		point.y = Mathf::RoundToInt(((position.y * invW) + 1.f) / 2.f * height);
		point.invZ = 1.f / (position.z * invW);
		point.invW = invW;
		return point;
	}
};

struct VertexBase
{
	u32 clipCode = 0x00;
	Vector4 position = Vector4();

	virtual Projection GetViewProjection(u32 width, u32 height)
	{
		return Projection::CalculateViewProjection(position, width, height);
	}

	static VertexBase Lerp(const VertexBase& a, const VertexBase& b, float t)
	{
		assert(0.f <= t && t <= 1.f);
		VertexBase out;
		out.position = Vector4::Lerp(a.position, b.position, t);
		out.clipCode = Clipper::CalculateClipCode(out.position);
		return out;
	}
};

struct VertexStd : VertexBase
{
	Vector3 normal = Vector3::up;
	Vector3 tangent = Vector3::right;
	Vector2 texcoord = Vector2::zero;

	static VertexStd Lerp(const VertexStd& a, const VertexStd& b, float t)
	{
		assert(0.f <= t && t <= 1.f);
		VertexStd out;
		out.position = Vector4::Lerp(a.position, b.position, t);
		out.normal = Vector3::Lerp(a.normal, b.normal, t);
		out.tangent = Vector3::Lerp(a.tangent, b.tangent, t);
		out.texcoord = Vector2::Lerp(a.texcoord, b.texcoord, t);
		out.clipCode = Clipper::CalculateClipCode(out.position);
		return out;
	}
};

template <typename VertexType>
struct Line
{
	VertexType v0;
	VertexType v1;

	Line(VertexType _v0, VertexType _v1) : v0(_v0), v1(_v1) {}
};

template <typename VertexType>
struct Triangle
{
	VertexType v0;
	VertexType v1;
	VertexType v2;

	Triangle(VertexType _v0, VertexType _v1, VertexType _v2) : v0(_v0), v1(_v1), v2(_v2) {}
};

}

#endif //! _BASE_CLIPPER_HPP_