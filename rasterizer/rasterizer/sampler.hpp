#ifndef _RASTERIZER_TEXTURE_SAMPLER_HPP_
#define _RASTERIZER_TEXTURE_SAMPLER_HPP_

#include "base/header.h"
#include "math/vector3.h"
#include "math/mathf.h"
#include "rasterizer/texture2d.h"

namespace rasterizer
{

struct WarpAddresser
{
	static float CalcAddress(float coord, int length)
	{
		return (coord - Mathf::Floor(coord)) * length - 0.5f;
	}
	static int FixAddress(int coord, int length)
	{
		if (coord < 0) return length - 1;
		if (coord >= length) return 0;
		return coord;
	}
};

struct ClampAddresser
{
	static float CalcAddress(float coord, int length)
	{
		return Mathf::Clamp(coord * length, 0.5f, length - 0.5f) - 0.5f;
	}
	static int FixAddress(int coord, int length)
	{
		if (coord < 0) return 0;
		if (coord >= length) return length - 1;
		return coord;
	}
};

struct MirrorAddresser
{
	static float CalcAddress(float coord, int length)
	{
		int round = Mathf::FloorToInt(coord);
		float tmpCoord = (round & 1) ? (1 + round - coord) : (coord - round);
		return tmpCoord * length - 0.5f;
	}
	static int FixAddress(int coord, int length)
	{
		if (coord < 0) return 0;
		if (coord >= (int)length) return length - 1;
		return coord;
	}
};

struct PointSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const Bitmap& bitmap, float u, float v)
	{
		int width = bitmap.GetWidth();
		int height = bitmap.GetHeight();

		float fx = XAddresserType::CalcAddress(u, width);
		int x = XAddresserType::FixAddress(Mathf::RoundToInt(fx), width);
		float fy = YAddresserType::CalcAddress(v, height);
		int y = YAddresserType::FixAddress(Mathf::RoundToInt(fy), height);

		return bitmap.GetPixel(x, y);
	}
};

struct LinearSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const Bitmap& bitmap, float u, float v)
	{
		int width = bitmap.GetWidth();
		int height = bitmap.GetHeight();

		float fx = XAddresserType::CalcAddress(u, width);
		int x0 = Mathf::FloorToInt(fx);
		float fy = YAddresserType::CalcAddress(v, height);
		int y0 = Mathf::FloorToInt(fy);
		float xFrac = fx - x0;
		float yFrac = fy - y0;
		x0 = XAddresserType::FixAddress(x0, width);
		y0 = YAddresserType::FixAddress(y0, height);
		int x1 = XAddresserType::FixAddress(x0 + 1, width);
		int y1 = YAddresserType::FixAddress(y0 + 1, height);

		Color c0 = bitmap.GetPixel(x0, y0);
		Color c1 = bitmap.GetPixel(x1, y0);
		Color c2 = bitmap.GetPixel(x0, y1);
		Color c3 = bitmap.GetPixel(x1, y1);

		return Color::Lerp(c0, c1, c2, c3, xFrac, yFrac);
	}
};

struct ProjectionSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static float Sample(const Bitmap& bitmap, float u, float v, float value, float bias)
	{
		int width = bitmap.GetWidth();
		int height = bitmap.GetHeight();

		float fx = XAddresserType::CalcAddress(u, width);
		int x0 = Mathf::FloorToInt(fx);
		float fy = YAddresserType::CalcAddress(v, height);
		int y0 = Mathf::FloorToInt(fy);
		float xFrac = fx - x0;
		float yFrac = fy - y0;
		x0 = XAddresserType::FixAddress(x0, width);
		y0 = YAddresserType::FixAddress(y0, height);
		int x1 = XAddresserType::FixAddress(x0 + 1, width);
		int y1 = YAddresserType::FixAddress(y0 + 1, height);

		float a = bitmap.GetAlpha(x0, y0) + bias < value ? 1.f : 0.f;
		float b = bitmap.GetAlpha(x1, y0) + bias < value ? 1.f : 0.f;
		float c = bitmap.GetAlpha(x0, y1) + bias < value ? 1.f : 0.f;
		float d = bitmap.GetAlpha(x1, y1) + bias < value ? 1.f : 0.f;
		return Mathf::BLerp(a, b, c, d, xFrac, yFrac);
	}
};

}

#endif //! _RASTERIZER_TEXTURE_SAMPLER_HPP_