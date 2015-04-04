#ifndef _BASE_TEXTURE_SAMPLER_HPP_
#define _BASE_TEXTURE_SAMPLER_HPP_

#include "base/header.h"
#include "math/vector3.h"
#include "math/mathf.h"
#include "rasterizer/texture.h"

namespace rasterizer
{

struct WarpAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		return (coord - Mathf::Floor(coord)) * (int)length - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return length - 1;
		if (coord >= (int)length) return 0;
		return coord;
	}
};

struct ClampAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		return Mathf::Clamp(coord * length, 0.5f, length - 0.5f) - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return 0;
		if (coord >= (int)length) return length - 1;
		return coord;
	}
};

struct MirrorAddresser
{
	static float CalcAddress(float coord, u32 length)
	{
		int round = Mathf::FloorToInt(coord);
		float tmpCoord = (round & 1) ? (1 + round - coord) : (coord - round);
		return tmpCoord * length - 0.5f;
	}
	static int FixAddress(int coord, u32 length)
	{
		if (coord < 0) return 0;
		if (coord >= (int)length) return length - 1;
		return coord;
	}
};

struct PointSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const Texture::Bitmap& bitmap, u32 width, u32 height, float u, float v)
	{
		float fx = XAddresserType::CalcAddress(u, width);
		int x = XAddresserType::FixAddress(Mathf::RoundToInt(fx), width);
		float fy = YAddresserType::CalcAddress(v, height);
		int y = YAddresserType::FixAddress(Mathf::RoundToInt(fy), height);

		return bitmap[y * width + x];
	}
};

struct LinearSampler
{
	template<typename XAddresserType, typename YAddresserType>
	static Color Sample(const Texture::Bitmap& bitmap, u32 width, u32 height, float u, float v)
	{
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

		Color c0 = bitmap[y0 * width + x0];
		Color c1 = bitmap[y0 * width + x1];
		Color c2 = bitmap[y1 * width + x0];
		Color c3 = bitmap[y1 * width + x1];

		return Color::Lerp(c0, c1, c2, c3, xFrac, yFrac);
	}
};

}

#endif //! _BASE_TEXTURE_SAMPLER_HPP_