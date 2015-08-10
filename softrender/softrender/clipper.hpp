#ifndef _RASTERIZER_CLIPPER_HPP_
#define _RASTERIZER_CLIPPER_HPP_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
//#include "softrender/vertex.hpp"

namespace sr
{

template <typename Type> struct Line;
template <typename Type> struct Triangle;
struct Clipper
{
	struct Plane
	{
		typedef bool(*GetClipCodeFunc)(const Vector4& v);
		typedef float(*ClippingFunc)(const Vector4& v0, const Vector4& v1);

		uint32_t cullMask = 0x0;
		GetClipCodeFunc getClipCodeFunc = nullptr;
		ClippingFunc clippingFunc = nullptr;
		Plane(uint32_t _cullMask, GetClipCodeFunc _getClipCodeFunc, ClippingFunc _clippingFunc) :
			cullMask(_cullMask),
			getClipCodeFunc(_getClipCodeFunc),
			clippingFunc(_clippingFunc) {}
	};

	static Plane viewFrustumPlanes[6];
	static uint32_t CalculateClipCode(const Vector4& hc)
	{
		uint32_t clipCode = 0x0;
		for (auto& p : viewFrustumPlanes)
		{
			if (p.getClipCodeFunc(hc))
			{
				clipCode |= p.cullMask;
			}
		}
		return clipCode;
	}

	template<typename Type>
	static std::vector<Line<Type> > ClipLine(const Type& v0, const Type& v1)
	{
		std::vector<Line<Type> > lines;
		if (0 != (v0.clipCode & v1.clipCode)) return lines;

		lines.emplace_back(v0, v1);
		if (0 == (v0.clipCode | v1.clipCode)) return lines;

		std::vector<Line<Type> > clippedLines;
		for (auto& p : viewFrustumPlanes)
		{
			clippedLines.clear();
			for (auto& l : lines)
			{
				Clipper::ClipLineFromPlane(clippedLines, l.v0, l.v1, p);
			}
			std::swap(lines, clippedLines);
		}
		return lines;
	}

	template<typename Type>
	static std::vector<Triangle<Type> > ClipTriangle(const Type& v0, const Type& v1, const Type& v2)
	{
		std::vector<Triangle<Type> > triangles;
		if (0 != (v0.clipCode & v1.clipCode & v2.clipCode)) return triangles;

		triangles.emplace_back(v0, v1, v2);
		if (0 == (v0.clipCode | v1.clipCode | v2.clipCode)) return triangles;

		std::vector<Triangle<Type> > clippedTriangles;
		for (auto& p : viewFrustumPlanes)
		//for (int i = 0; i < 2; ++i)
		{
			//Plane& p = viewFrustumPlanes[i];
			clippedTriangles.clear();
			for (auto& f : triangles)
			{
				Clipper::ClipTriangleFromPlane(clippedTriangles, f.v0, f.v1, f.v2, p);
			}
			std::swap(triangles, clippedTriangles);
		}
		return triangles;
	}

	template<typename Type>
	static void ClipLineFromPlane(std::vector<Line<Type> >& clippedLines,
		const Type& v0, const Type& v1, const Plane& plane)
	{
		if (v0.clipCode & v1.clipCode & plane.cullMask) return;
		else if (0 == ((v0.clipCode | v1.clipCode) & plane.cullMask))
		{
			clippedLines.emplace_back(v0, v1);
		}
		else if ((~v0.clipCode) & v1.clipCode & plane.cullMask)
		{
			float t = plane.clippingFunc(v1.position, v0.position);
			assert(0.f <= t && t <= 1.f);
			clippedLines.emplace_back(v0, Type::LinearInterp(v1, v0, t));
		}
		else if (v0.clipCode & (~v1.clipCode) & plane.cullMask)
		{
			float t = plane.clippingFunc(v0.position, v1.position);
			assert(0.f <= t && t <= 1.f);
			clippedLines.emplace_back(Type::LinearInterp(v0, v1, t), v1);
		}
	}

	template<typename Type>
	static void ClipTriangleFromPlane(std::vector<Triangle<Type> >& clippedTriangles,
		const Type& v0, const Type& v1, const Type& v2, const Plane& plane)
	{
		if (v0.clipCode & v1.clipCode & v2.clipCode & plane.cullMask) return;
		else if (0 == ((v0.clipCode | v1.clipCode | v2.clipCode) & plane.cullMask))
		{
			clippedTriangles.emplace_back(v0, v1, v2);
		}
		else if ((~v0.clipCode) & v1.clipCode & v2.clipCode & plane.cullMask)
		{
			ClipTriangleWithTwoVertexOut(clippedTriangles, v0, v1, v2, plane);
		}
		else if (v0.clipCode & (~v1.clipCode) & v2.clipCode & plane.cullMask)
		{
			ClipTriangleWithTwoVertexOut(clippedTriangles, v1, v2, v0, plane);
		}
		else if (v0.clipCode & v1.clipCode & (~v2.clipCode) & plane.cullMask)
		{
			ClipTriangleWithTwoVertexOut(clippedTriangles, v2, v0, v1, plane);
		}
		else if ((~v0.clipCode) & (~v1.clipCode) & v2.clipCode & plane.cullMask)
		{
			ClipTriangleWithOneVertexOut(clippedTriangles, v0, v1, v2, plane);
		}
		else if (v0.clipCode & (~v1.clipCode) & (~v2.clipCode) & plane.cullMask)
		{
			ClipTriangleWithOneVertexOut(clippedTriangles, v1, v2, v0, plane);
		}
		else if ((~v0.clipCode) & v1.clipCode & (~v2.clipCode) & plane.cullMask)
		{
			ClipTriangleWithOneVertexOut(clippedTriangles, v2, v0, v1, plane);
		}
	}

	template<typename Type>
	static void ClipTriangleWithOneVertexOut(std::vector<Triangle<Type> >& clippedTriangles,
		const Type& v0, const Type& v1, const Type& v2, const Plane& plane)
	{
		// v0 & v1 in, v2 out
		float t1 = plane.clippingFunc(v2.position, v1.position);
		float t0 = plane.clippingFunc(v2.position, v0.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t0 && t0 <= 1.f);
		Type tv0 = Type::LinearInterp(v2, v1, t1);
		Type tv1 = Type::LinearInterp(v2, v0, t0);
		clippedTriangles.emplace_back(v0, v1, tv0);
		clippedTriangles.emplace_back(v0, tv0, tv1);
	}

	template<typename Type>
	static void ClipTriangleWithTwoVertexOut(std::vector<Triangle<Type> >& clippedTriangles,
		const Type& v0, const Type& v1, const Type& v2, const Plane& plane)
	{
		// v0 in, v1, v2 out
		float t1 = plane.clippingFunc(v1.position, v0.position);
		float t2 = plane.clippingFunc(v2.position, v0.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		Type tv0 = Type::LinearInterp(v1, v0, t1);
		Type tv1 = Type::LinearInterp(v2, v0, t2);
		clippedTriangles.emplace_back(v0, tv0, tv1);
	}

	static float Clip(float f0, float w0, float f1, float w1)
	{
		return (w0 - f0) / ((w0 - f0) - (w1 - f1));
	}

};

}

#endif //! _RASTERIZER_CLIPPER_HPP_