#ifndef _BASE_COLOR_INLINE_
#define _BASE_COLOR_INLINE_

#include "math/mathf.h"

namespace rasterizer
{
    
ColorRGB ColorRGB::operator +(const ColorRGB& v) const { return ColorRGB(r + v.r, g + v.g, b + v.b); }
ColorRGB ColorRGB::operator += (const ColorRGB& v) { return (*this) = (*this) + v; }
ColorRGB ColorRGB::operator *(float f) const { return ColorRGB(r * f, g * f, b * f); }
ColorRGB ColorRGB::operator *(const ColorRGB& v) const { return ColorRGB(r * v.r, g * v.g, b * v.b); }
ColorRGB ColorRGB::operator *= (float f) { return (*this) = (*this) * f; }
ColorRGB ColorRGB::operator *= (const ColorRGB& v) { return (*this) = (*this) * v; }

ColorRGB::operator Vector3() const
{
	return Vector3(r, g, b);
}

#if _NOCRASH_
Color Color::operator +(const Color& v) const { return Color(_mm_add_ps(m, v.m)); }
Color Color::operator *(float f) const { return Color(_mm_mul_ps(m, _mm_set1_ps(s))); }
Color Color::operator *(const Color& v) const { return Color(_mm_mul_ps(m, v.m)); }
#else
Color Color::operator +(const Color& v) const { return Color(a + v.a, r + v.r, g + v.g, b + v.b); }
Color Color::operator *(float f) const { return Color(a * f, r * f, g * f, b * f); }
Color Color::operator *(const Color& v) const { return Color(a * v.a, r * v.r, g * v.g, b * v.b); }
#endif
Color Color::operator += (const Color& v) { return (*this) = (*this) + v; }
Color Color::operator *= (float f) { return (*this) = (*this) * f; }
Color Color::operator *= (const Color& v) { return (*this) = (*this) * v; }

Color Color::Lerp(const Color& a, const Color& b, float t)
{
	Color color;
#if _NOCRASH_
	static __m128 _ps1 = _mm_set1_ps(1.f);
	__m128 t2 = _mm_set1_ps(t);
	__m128 t1 = _mm_sub_ps(_ps1, t2);
	color.m = _mm_add_ps(_mm_mul_ps(a.m, t1), _mm_mul_ps(b.m, t2));
#else
	color.a = Mathf::Lerp(a.a, b.a, t);
	color.r = Mathf::Lerp(a.r, b.r, t);
	color.g = Mathf::Lerp(a.g, b.g, t);
	color.b = Mathf::Lerp(a.b, b.b, t);
#endif
	return color;
}


Color Color::Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2)
{
#if _NOCRASH_
	static __m128 _ps1 = _mm_set1_ps(1.f);
	__m128 mt1 = _mm_set1_ps(t1);
	__m128 mt2 = _mm_set1_ps(t2);
	__m128 mrt1 = _mm_sub_ps(_ps1, mt1);
	__m128 mrt2 = _mm_sub_ps(_ps1, mt2);
	__m128 tmp1 = _mm_add_ps(_mm_mul_ps(a.m, mrt1), _mm_mul_ps(b.m, mt1));
	__m128 tmp2 = _mm_add_ps(_mm_mul_ps(c.m, mrt1), _mm_mul_ps(d.m, mt1));

	return Color(_mm_add_ps(_mm_mul_ps(tmp1, mrt2), _mm_mul_ps(tmp2, mt2)));
#else
	return Color::Lerp(Color::Lerp(a, b, t1), Color::Lerp(c, d, t1), t2);
#endif
}

Color::operator Color32() const
{
    Color32 color;
	Color col = Clamp();
#if _NOCRASH_
    static __m128 _ps255 = _mm_set1_ps(255.f);
    __m128i im = _mm_cvtps_epi32(_mm_mul_ps(col.m, _ps255));
    color.r = _mm_extract_epi8(im, 0);
    color.g = _mm_extract_epi8(im, 4);
    color.b = _mm_extract_epi8(im, 8);
    color.a = _mm_extract_epi8(im, 12);
#else
	color.a = (uint8_t)(col.a * 255);
	color.r = (uint8_t)(col.r * 255);
	color.g = (uint8_t)(col.g * 255);
	color.b = (uint8_t)(col.b * 255);
#endif
    return color;
}

Color::operator Vector4() const
{
	return Vector4(r, g, b, a);
}


Color Color::Clamp() const
{
#if _NOCRASH_
	return Color(_mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m)));
#else
	return Color(
		Mathf::Clamp01(a),
		Mathf::Clamp01(r),
		Mathf::Clamp01(g),
		Mathf::Clamp01(b));
#endif
}

Color Color::Inverse() const
{
#if _NOCRASH_
	return Color(_mm_sub_ps(_mm_set1_ps(1.f), m));
#else
	return Color(1.f - a, 1.f - r, 1.f - g, 1.f - b);
#endif
}

Color32::operator Color() const
{
	Color color;
#if _NOCRASH_
    static __m128 _ps1div255 = _mm_set1_ps(1.f / 255.f);
	__m128 m = _mm_cvtepi32_ps(_mm_cvtepu8_epi32(_mm_set1_epi32(rgba)));
    color.m = _mm_mul_ps(m, _ps1div255);
#else
    color.a = a / 255.f;
    color.r = r / 255.f;
    color.g = g / 255.f;
    color.b = b / 255.f;
#endif
    return color;
}

}

#endif // !_BASE_COLOR_INLINE_