#ifndef _MATH_COLOR_INLINE_
#define _MATH_COLOR_INLINE_

#include "math/mathf.h"

namespace sr
{

Color Color::operator +(float f) const { return Color(a + f, r + f, g + f, b + f); }
Color Color::operator +(const Color& v) const { return Color(a + v.a, r + v.r, g + v.g, b + v.b); }
Color Color::operator *(float f) const { return Color(a * f, r * f, g * f, b * f); }
Color Color::operator *(const Color& v) const { return Color(a * v.a, r * v.r, g * v.g, b * v.b); }
Color Color::operator += (float f) { return (*this) = (*this) + f; }
Color Color::operator += (const Color& v) { return (*this) = (*this) + v; }
Color Color::operator *= (float f) { return (*this) = (*this) * f; }
Color Color::operator *= (const Color& v) { return (*this) = (*this) * v; }

Color Color::Lerp(const Color& a, const Color& b, float t)
{
	Color color;
	color.a = Mathf::Lerp(a.a, b.a, t);
	color.r = Mathf::Lerp(a.r, b.r, t);
	color.g = Mathf::Lerp(a.g, b.g, t);
	color.b = Mathf::Lerp(a.b, b.b, t);
	return color;
}

Color Color::Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2)
{
	return Color::Lerp(Color::Lerp(a, b, t1), Color::Lerp(c, d, t1), t2);
}

Color::operator Color32() const
{
	Color32 color;
	Color col = Clamp();
	color.a = (uint8_t)(col.a * 255);
	color.r = (uint8_t)(col.r * 255);
	color.g = (uint8_t)(col.g * 255);
	color.b = (uint8_t)(col.b * 255);
	return color;
}

Color::operator Vector4() const
{
	return Vector4(r, g, b, a);
}


Color Color::Clamp() const
{
	return Color(
		Mathf::Clamp01(a),
		Mathf::Clamp01(r),
		Mathf::Clamp01(g),
		Mathf::Clamp01(b));
}

Color32::operator Color() const
{
	Color color;
	color.a = a / 255.f;
	color.r = r / 255.f;
	color.g = g / 255.f;
	color.b = b / 255.f;
	return color;
}

float Color::GammaToLinearSpaceExact(float value)
{
	if (value <= 0.04045f)
		return value / 12.92f;
	else if (value < 1.0f)
		return Mathf::Pow((value + 0.055f) / 1.055f, 2.4f);
	else
		return Mathf::Pow(value, 2.2f);
}

Color Color::GammaToLinearSpace(const Color& c)
{
	Color color;
	color.a = c.a;
	color.r = GammaToLinearSpaceExact(c.r);
	color.g = GammaToLinearSpaceExact(c.g);
	color.b = GammaToLinearSpaceExact(c.b);
	return color;
}

Color Color::GammaToLinearSpaceFast(const Color& c)
{
	return c * (c * (c * 0.305306011f + 0.682171111f) + 0.012522878f);
}

float Color::LinearToGammaSpaceExact(float value)
{
	if (value <= 0.0f)
		return 0.0f;
	else if (value <= 0.0031308f)
		return 12.92f * value;
	else if (value < 1.0f)
		return 1.055f * Mathf::Pow(value, 0.4166667f) - 0.055f;
	else
		return Mathf::Pow(value, 0.45454545f);
}

Color Color::LinearToGammaSpace(const Color& c)
{
	Color color;
	color.a = c.a;
	color.r = LinearToGammaSpaceExact(c.r);
	color.g = LinearToGammaSpaceExact(c.g);
	color.b = LinearToGammaSpaceExact(c.b);
	return color;
}

Color Color::LinearToGammaSpaceFast(const Color& c)
{
	Color color;
	color.a = c.a;
	color.r = Mathf::Max(1.055f * Mathf::Pow(Mathf::Max(c.r, 0.f), 0.416666667f) - 0.055f, 0.f);
	color.g = Mathf::Max(1.055f * Mathf::Pow(Mathf::Max(c.g, 0.f), 0.416666667f) - 0.055f, 0.f);
	color.b = Mathf::Max(1.055f * Mathf::Pow(Mathf::Max(c.b, 0.f), 0.416666667f) - 0.055f, 0.f);
	return color;
}

}

#endif // !_MATH_COLOR_INLINE_