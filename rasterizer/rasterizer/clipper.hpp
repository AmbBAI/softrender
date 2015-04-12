#ifndef _RASTERIZER_CLIPPER_HPP_
#define _RASTERIZER_CLIPPER_HPP_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
//#include "rasterizer/vertex.hpp"

namespace rasterizer
{

template <typename VertexType> struct Line;
template <typename VertexType> struct Triangle;
struct Clipper
{
	struct Plane
	{
		typedef bool(*GetClipCodeFunc)(const Vector4& v);
		typedef float(*ClippingFunc)(const Vector4& v0, const Vector4& v1);

		u32 cullMask = 0x0;
		GetClipCodeFunc getClipCodeFunc = nullptr;
		ClippingFunc clippingFunc = nullptr;
		Plane(u32 _cullMask, GetClipCodeFunc _getClipCodeFunc, ClippingFunc _clippingFunc) :
			cullMask(_cullMask),
			getClipCodeFunc(_getClipCodeFunc),
			clippingFunc(_clippingFunc) {}
	};

	static Plane viewFrustumPlanes[6];
	static u32 CalculateClipCode(const Vector4& hc)
	{
		float w = hc.w;
		u32 clipCode = 0x0;
		for (auto& p : viewFrustumPlanes)
		{
			if (p.getClipCodeFunc(hc))
			{
				clipCode |= p.cullMask;
			}
		}
		return clipCode;
	}

	template<typename VertexType>
	static std::vector<Line<VertexType> > ClipLine(const VertexType& v0, const VertexType& v1)
	{
		std::vector<Line<VertexType> > lines;
		if (0 != (v0.clipCode & v1.clipCode)) return lines;

		lines.push_back(Line<VertexType>(v0, v1));
		if (0 == (v0.clipCode | v1.clipCode)) return lines;

		std::vector<Line<VertexType> > clippedLines;
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

	template<typename VertexType>
	static std::vector<Triangle<VertexType> > ClipTriangle(const VertexType& v0, const VertexType& v1, const VertexType& v2)
	{
		std::vector<Triangle<VertexType> > triangles;
		if (0 != (v0.clipCode & v1.clipCode & v2.clipCode)) return triangles;

		triangles.push_back(Triangle<VertexType>(v0, v1, v2));
		if (0 == (v0.clipCode | v1.clipCode | v2.clipCode)) return triangles;

		std::vector<Triangle<VertexType> > clippedTriangles;
		for (auto& p : viewFrustumPlanes)
			//for (int i = 0; i < 6; ++i)
		{
			//	Plane& p = viewFrustumPlanes[i];
			clippedTriangles.clear();
			for (auto& f : triangles)
			{
				//printf("%f, %f, %f, %f\n", f.v0.hc.x, f.v0.hc.y, f.v0.hc.z, f.v0.hc.w);
				//printf("%f, %f, %f, %f\n", f.v1.hc.x, f.v1.hc.y, f.v1.hc.z, f.v1.hc.w);
				//printf("%f, %f, %f, %f\n", f.v2.hc.x, f.v2.hc.y, f.v2.hc.z, f.v2.hc.w);
				//printf("--------------\n");
				Clipper::ClipTriangleFromPlane(clippedTriangles, f.v0, f.v1, f.v2, p);
			}
			//printf("==========\n");
			std::swap(triangles, clippedTriangles);
		}
		return triangles;
	}

	template<typename VertexType>
	static void ClipLineFromPlane(std::vector<Line<VertexType> >& clippedLines,
		const VertexType& v0, const VertexType& v1, const Plane& plane)
	{
		if (v0.clipCode & v1.clipCode & plane.cullMask) return;
		else if (0 == ((v0.clipCode | v1.clipCode) & plane.cullMask))
		{
			clippedLines.push_back(Line<VertexType>(v0, v1));
		}
		else if ((~v0.clipCode) & v1.clipCode & plane.cullMask)
		{
			float t = plane.clippingFunc(v1.hc, v0.hc);
			assert(0.f <= t && t <= 1.f);
			clippedLines.push_back(Line<VertexType>(v0, VertexType::Lerp(v1, v0, t)));
		}
		else if (v0.clipCode & (~v1.clipCode) & plane.cullMask)
		{
			float t = plane.clippingFunc(v0.hc, v1.hc);
			assert(0.f <= t && t <= 1.f);
			clippedLines.push_back(Line<VertexType>(VertexType::Lerp(v0, v1, t), v1));
		}
	}

	template<typename VertexType>
	static void ClipTriangleFromPlane(std::vector<Triangle<VertexType> >& clippedTriangles,
		const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane)
	{
		if (v0.clipCode & v1.clipCode & v2.clipCode & plane.cullMask) return;
		else if (0 == ((v0.clipCode | v1.clipCode | v2.clipCode) & plane.cullMask))
		{
			clippedTriangles.push_back(Triangle<VertexType>(v0, v1, v2));
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

	template<typename VertexType>
	static void ClipTriangleWithOneVertexOut(std::vector<Triangle<VertexType> >& clippedTriangles,
		const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane)
	{
		// v0 & v1 in, v2 out
		float t1 = plane.clippingFunc(v2.hc, v1.hc);
		float t0 = plane.clippingFunc(v2.hc, v0.hc);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t0 && t0 <= 1.f);
		VertexType tv0 = VertexType::Lerp(v2, v1, t1);
		VertexType tv1 = VertexType::Lerp(v2, v0, t0);
		clippedTriangles.push_back(Triangle<VertexType>(v0, v1, tv0));
		clippedTriangles.push_back(Triangle<VertexType>(v0, tv0, tv1));
	}

	template<typename VertexType>
	static void ClipTriangleWithTwoVertexOut(std::vector<Triangle<VertexType> >& clippedTriangles,
		const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane)
	{
		// v0 in, v1, v2 out
		float t1 = plane.clippingFunc(v1.hc, v0.hc);
		float t2 = plane.clippingFunc(v2.hc, v0.hc);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		VertexType tv0 = VertexType::Lerp(v1, v0, t1);
		VertexType tv1 = VertexType::Lerp(v2, v0, t2);
		clippedTriangles.push_back(Triangle<VertexType>(v0, tv0, tv1));
	}

	static float Clip(float f0, float w0, float f1, float w1)
	{
		return (w0 - f0) / ((w0 - f0) - (w1 - f1));
	}
};

}

#endif //! _RASTERIZER_CLIPPER_HPP_