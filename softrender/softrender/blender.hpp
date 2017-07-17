#ifndef _SOFTRENDER_BLENDER_H_
#define _SOFTRENDER_BLENDER_H_

#include "base/header.h"
#include "math/color.h"

namespace sr
{

class Blender
{
public:
	enum BlendMode
	{
		BlendMode_One,
		BlendMode_Zero,
		BlendMode_SrcColor,
		BlendMode_SrcAlpha,
		BlendMode_DstColor,
		BlendMode_DstAlpha,
		BlendMode_OneMinusSrcColor,
		BlendMode_OneMinusSrcAlpha,
		BlendMode_OneMinusDstColor,
		BlendMode_OneMinusDstAlpha,
		BlendMode_SrcAlphaSaturate,
	};

	enum BlendOP
	{
		BlendOP_Add,
		BlendOP_Subtract,
		BlendOP_ReverseSubtract,
		BlendOP_Min,
		BlendOP_Max,
	};

	BlendOP colorOP = BlendOP_Add;
	BlendMode srcColorMode = BlendMode_SrcAlpha;
	BlendMode dstColorMode = BlendMode_OneMinusSrcAlpha;
	BlendOP alphaOP = BlendOP_Add;
	BlendMode srcAlphaMode = BlendMode_SrcAlpha;
	BlendMode dstAlphaMode = BlendMode_OneMinusSrcAlpha;

	std::function<Color(const Color&, const Color&)> blendOP;
	std::function<Color(const Color&, const Color&)> srcBlendFactor;
	std::function<Color(const Color&, const Color&)> dstBlendFactor;

	void SetColorBlendMode(BlendMode srcBlendMode, BlendMode dstBlendMode)
	{
		this->srcColorMode = srcBlendMode;
		this->dstColorMode = dstBlendMode;
	}

	void SetAlphaBlendMode(BlendMode srcBlendMode, BlendMode dstBlendMode)
	{
		this->srcAlphaMode = srcBlendMode;
		this->dstAlphaMode = dstBlendMode;
	}

	Color Blend(const Color& src, const Color& dst) const
	{
		return ApplyBlendOP(GetSrcBlendFactor(src, dst) * src, GetDstBlendFactor(src, dst) * dst);
	}

	Color ApplyBlendOP(const Color& src, const Color& dst) const
	{
		if (blendOP != nullptr)
		{
			return blendOP(src, dst);
		}

		Color color = Color::clear;
		color.rgb = _BlendRGBOP(colorOP, src.rgb, dst.rgb);
		color.a = _BlendAlphaOP(alphaOP, src.a, dst.a);
		return color;
	}

	Color GetSrcBlendFactor(const Color& src, const Color& dst) const
	{
		if (srcBlendFactor)
		{
			return srcBlendFactor(src, dst);
		}

		Color color = Color::clear;
		color.rgb = _BlendRGBFactor(srcColorMode, src, dst);
		color.a = _BlendAlphaFactor(srcAlphaMode, src, dst);
		return color;
	}

	Color GetDstBlendFactor(const Color& src, const Color& dst) const
	{
		if (dstBlendFactor != nullptr)
		{
			return dstBlendFactor(src, dst);
		}

		Color color = Color::clear;
		color.rgb = _BlendRGBFactor(dstColorMode, src, dst);
		color.a = _BlendAlphaFactor(dstAlphaMode, src, dst);
		return color;
	}

private:
	static Vector3 _BlendRGBOP(BlendOP op, const Vector3& a, const Vector3& b)
	{
		switch (op)
		{
		case BlendOP_Add:
			return a + b;
		case BlendOP_Subtract:
			return a - b;
		case BlendOP_Min:
			return Vector3::Min(a, b);
		case BlendOP_Max:
			return Vector3::Max(a, b);
		default:
			break;
		}
		return a + b;
	}

	static float _BlendAlphaOP(BlendOP op, const float a, const float b)
	{
		switch (op)
		{
		case BlendOP_Add:
			return a + b;
		case BlendOP_Subtract:
			return a - b;
		case BlendOP_Min:
			return Mathf::Min(a, b);
		case BlendOP_Max:
			return Mathf::Max(a, b);
		default:
			break;
		}
		return a + b;
	}

	static Vector3 _BlendRGBFactor(BlendMode mode, const Color& src, const Color& dst)
	{
		switch (mode)
		{
		case BlendMode_Zero:
			return Vector3::zero;
		case BlendMode_SrcColor:
			return src.rgb;
		case BlendMode_SrcAlpha:
			return Vector3::one * src.a;
		case BlendMode_DstColor:
			return dst.rgb;
		case BlendMode_DstAlpha:
			return Vector3::one * dst.a;
		case BlendMode_OneMinusSrcColor:
			return Vector3::one - src.rgb;
		case BlendMode_OneMinusSrcAlpha:
			return Vector3::one * (1.f - src.a);
		case BlendMode_OneMinusDstColor:
			return Vector3::one - dst.rgb;
		case BlendMode_OneMinusDstAlpha:
			return Vector3::one * (1.f - dst.a);
		case BlendMode_SrcAlphaSaturate:
			return Vector3::one * Mathf::Min(src.a, 1 - dst.a);
		case BlendMode_One:
		default:
			return Vector3::one;
		}
	}

	static float _BlendAlphaFactor(BlendMode mode, const Color& src, const Color& dst)
	{
		switch (mode)
		{
		case BlendMode_Zero:
			return 0.f;
		case BlendMode_SrcAlpha:
		case BlendMode_SrcColor:
			return src.a;
		case BlendMode_DstAlpha:
		case BlendMode_DstColor:
			return dst.a;
		case BlendMode_OneMinusSrcAlpha:
		case BlendMode_OneMinusSrcColor:
			return 1.f - src.a;
		case BlendMode_OneMinusDstAlpha:
		case BlendMode_OneMinusDstColor:
			return 1.f - dst.a;
		case BlendMode_SrcAlphaSaturate:
		case BlendMode_One:
		default:
			return 1.f;
		}
	}
};


} // namespace sr

#endif // !_SOFTRENDER_BLENDER_H_
