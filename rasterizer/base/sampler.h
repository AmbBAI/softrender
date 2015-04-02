#ifndef _BASE_TEXTURE_SAMPLER_H_
#define _BASE_TEXTURE_SAMPLER_H_

#include "base/header.h"
#include "base/texture.h"
#include "math/vector3.h"
#include "math/mathf.h"

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
		assert(height > 0 && width > 0);

		float fx = XAddresserType::CalcAddress(u, width);
		int x = Mathf::FloorToInt(fx);
		float fy = YAddresserType::CalcAddress(v, height);
		int y = Mathf::FloorToInt(fy);
		float xFrac = fx - x;
		float yFrac = fy - y;
		x = XAddresserType::FixAddress(x, width);
		y = YAddresserType::FixAddress(y, height);
		int x2 = XAddresserType::FixAddress(x + 1, width);
		int y2 = YAddresserType::FixAddress(y + 1, height);

		Color c0 = bitmap[y * width + x];
		Color c1 = bitmap[y * width + x2];
		Color c2 = bitmap[y2 * width + x];
		Color c3 = bitmap[y2 * width + x2];

		return Color::Lerp(Color::Lerp(c0, c1, xFrac), Color::Lerp(c2, c3, xFrac), yFrac);
	}
};

}

#endif //! _BASE_TEXTURE_SAMPLER_H_