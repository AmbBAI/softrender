#ifndef _SOFTRENDER_RASTERIZER_H_
#define _SOFTRENDER_RASTERIZER_H_

#include "base/header.h"
#include "math/mathf.h"
#include "math/color.h"
#include "softrender/srtypes.hpp"

namespace sr
{

struct RasterizerInfo
{
	int x, y;
	float depth;
	float wx, wy, wz;
};

struct Rasterizer2x2Info
{
	int x, y;
	uint8_t maskCode;
#if _MATH_SIMD_INTRINSIC_
	SIMD_ALIGN float depth[4];
	SIMD_ALIGN float wx[4], wy[4], wz[4];
#else
	float depth[4];
	float wx[4], wy[4], wz[4];
#endif
};

class Rasterizer
{
private:
	int width = 0;
	int height = 0;

public:
	Rasterizer() = default;

	void Initlize(int width, int height)
	{
		this->width = width;
		this->height = height;
	}
	
	template<typename Type>
	using Render2x2Func = std::function<void(const Type&, const Rasterizer2x2Info&)>;

	//static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color)
	//{
	//	bool steep = Mathf::Abs(y1 - y0) > Mathf::Abs(x1 - x0);
	//	if (steep)
	//	{
	//		std::swap(x0, y0);
	//		std::swap(x1, y1);
	//	}
	//	if (x0 > x1)
	//	{
	//		std::swap(x0, x1);
	//		std::swap(y0, y1);
	//	}

	//	int deltaX = x1 - x0;
	//	int deltaY = Mathf::Abs(y1 - y0);
	//	int error = deltaX / 2;
	//	int yStep = -1;
	//	if (y0 < y1) yStep = 1;

	//	for (int x = x0, y = y0; x < x1; ++x)
	//	{
	//		steep ? colorBuffer->SetPixel(y, x, color) : colorBuffer->SetPixel(x, y, color);
	//		error = error - deltaY;
	//		if (error < 0)
	//		{
	//			y = y + yStep;
	//			error = error + deltaX;
	//		}
	//	}
	//}

	template<typename DrawDataType>
	void RasterizerTriangle(Triangle<Projection> projection, Render2x2Func<DrawDataType> renderFunc, const DrawDataType& renderData)
	{
		const Projection& p0 = projection.v0;
		const Projection& p1 = projection.v1;
		const Projection& p2 = projection.v2;

		int minX = Mathf::Min(p0.x, p1.x, p2.x);
		int minY = Mathf::Min(p0.y, p1.y, p2.y);
		int maxX = Mathf::Max(p0.x, p1.x, p2.x);
		int maxY = Mathf::Max(p0.y, p1.y, p2.y);

		if (minX < 0) minX = 0;
		if (minY < 0) minY = 0;
		if (maxX >= width) maxX = width - 1;
		if (maxY >= height) maxY = height - 1;

		if (maxX < minX) return;
		if (maxY < minY) return;

		int dx01 = p1.x - p0.x;
		int dx12 = p2.x - p1.x;
		int dx20 = p0.x - p2.x;

		int dy01 = p1.y - p0.y;
		int dy12 = p2.y - p1.y;
		int dy20 = p0.y - p2.y;

		int startW0 = Projection::Orient2D(p1.x, p1.y, p0.x, p0.y, minX, minY);
		int startW1 = Projection::Orient2D(p2.x, p2.y, p1.x, p1.y, minX, minY);
		int startW2 = Projection::Orient2D(p0.x, p0.y, p2.x, p2.y, minX, minY);

		if (dy01 < 0 || (dy01 == 0 && dx01 < 0)) startW0 += 1;
		if (dy12 < 0 || (dy12 == 0 && dx12 < 0)) startW1 += 1;
		if (dy20 < 0 || (dy20 == 0 && dx20 < 0)) startW2 += 1;

		Rasterizer2x2Info info;
#if _MATH_SIMD_INTRINSIC_
		static __m128 _mf_one = _mm_set1_ps(1.f);

		__m128i mi_w0_delta = _mm_setr_epi32(0, dy01, -dx01, dy01 - dx01);
		__m128i mi_w1_delta = _mm_setr_epi32(0, dy12, -dx12, dy12 - dx12);
		__m128i mi_w2_delta = _mm_setr_epi32(0, dy20, -dx20, dy20 - dx20);

		__m128 mf_p0_invW = _mm_load1_ps(&(p0.invW));
		__m128 mf_p1_invW = _mm_load1_ps(&(p1.invW));
		__m128 mf_p2_invW = _mm_load1_ps(&(p2.invW));
#else
		int i_w0_delta[4] = { 0, dy01, -dx01, dy01 - dx01 };
		int i_w1_delta[4] = { 0, dy12, -dx12, dy12 - dx12 };
		int i_w2_delta[4] = { 0, dy20, -dx20, dy20 - dx20 };
#endif

		for (int y = minY; y <= maxY; y += 2)
		{
			int w0 = startW0;
			int w1 = startW1;
			int w2 = startW2;

			for (int x = minX; x <= maxX; x += 2)
			{
				info.maskCode = 0x00;
#if _MATH_SIMD_INTRINSIC_
				__m128i mi_w0 = _mm_add_epi32(_mm_set1_epi32(w0), mi_w0_delta);
				__m128i mi_w1 = _mm_add_epi32(_mm_set1_epi32(w1), mi_w1_delta);
				__m128i mi_w2 = _mm_add_epi32(_mm_set1_epi32(w2), mi_w2_delta);

				__m128i mi_or_w = _mm_or_si128(_mm_or_si128(mi_w0, mi_w1), mi_w2);
				SIMD_ALIGN static int or_w[4];
				_mm_store_si128((__m128i*)or_w, mi_or_w);
				if (or_w[0] > 0) info.maskCode |= 0x1;
				if (or_w[1] > 0) info.maskCode |= 0x2;
				if (or_w[2] > 0) info.maskCode |= 0x4;
				if (or_w[3] > 0) info.maskCode |= 0x8;
				if (x + 1 >= maxX) info.maskCode &= ~0xA;
				if (y + 1 >= maxY) info.maskCode &= ~0xC;

				if (info.maskCode != 0)
				{
					__m128 mf_w0 = _mm_cvtepi32_ps(mi_w0);
					__m128 mf_w1 = _mm_cvtepi32_ps(mi_w1);
					__m128 mf_w2 = _mm_cvtepi32_ps(mi_w2);

					__m128 mf_tmp0 = _mm_mul_ps(mf_w1, mf_p0_invW);
					__m128 mf_tmp1 = _mm_mul_ps(mf_w2, mf_p1_invW);
					__m128 mf_tmp2 = _mm_mul_ps(mf_w0, mf_p2_invW);
					//__m128 mf_invw = _mm_rcp_ps(_mm_add_ps(_mm_add_ps(mf_x, mf_y), mf_z));
					__m128 mf_invSum = _mm_div_ps(_mf_one, _mm_add_ps(_mm_add_ps(mf_tmp0, mf_tmp1), mf_tmp2));
					_mm_store_ps(info.wx, _mm_mul_ps(mf_tmp0, mf_invSum));
					_mm_store_ps(info.wy, _mm_mul_ps(mf_tmp1, mf_invSum));
					_mm_store_ps(info.wz, _mm_mul_ps(mf_tmp2, mf_invSum));

					for (int i = 0; i < 4; ++i)
					{
#else
				int i_w0[4], i_w1[4], i_w2[4];
				for (int i = 0; i < 4; ++i)
				{
					i_w0[i] = w0 + i_w0_delta[i];
					i_w1[i] = w1 + i_w1_delta[i];
					i_w2[i] = w2 + i_w2_delta[i];
					if ((i_w0[i] | i_w1[i] | i_w2[i]) > 0) info.maskCode |= (1 << i);
				}
				if (x + 1 >= maxX) info.maskCode &= ~0xA;
				if (y + 1 >= maxY) info.maskCode &= ~0xC;

				if (info.maskCode != 0)
				{
					for (int i = 0; i < 4; ++i)
					{
						float f_w0 = (float)i_w0[i];
						float f_w1 = (float)i_w1[i];
						float f_w2 = (float)i_w2[i];

						info.wx[i] = f_w1 * p0.invW;
						info.wy[i] = f_w2 * p1.invW;
						info.wz[i] = f_w0 * p2.invW;
						float f_invSum = 1.f / (info.wx[i] + info.wy[i] + info.wz[i]);
						info.wx[i] *= f_invSum;
						info.wy[i] *= f_invSum;
						info.wz[i] *= f_invSum;
#endif
						info.depth[i] = Mathf::TriangleInterp(p0.z, p1.z, p2.z, info.wx[i], info.wy[i], info.wz[i]);
					}

					info.x = x;
					info.y = y;
					renderFunc(renderData, info);
				}


				w0 += dy01 * 2;
				w1 += dy12 * 2;
				w2 += dy20 * 2;
			}

			startW0 -= dx01 * 2;
			startW1 -= dx12 * 2;
			startW2 -= dx20 * 2;
		}
	}


};

}

#endif // _SOFTRENDER_RASTERIZER_H_