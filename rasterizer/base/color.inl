#ifndef _BASE_COLOR_INLINE_
#define _BASE_COLOR_INLINE_

#include "math/mathf.h"

namespace rasterizer
{
    
const Color Color::Add(const Color& c) const
{
	return Color(a + c.a, r + c.r, g + c.g, b + c.b);
}

const Color Color::Multiply(float s) const
{
#if _MATH_SIMD_INTRINSIC_
    return Color(_mm_mul_ps(m, _mm_set1_ps(s)));
#else
	return Color(a * s, r * s, g * s, b * s);
#endif
}

const Color Color::Modulate(const Color& c) const
{
#if _MATH_SIMD_INTRINSIC_
    return Color(_mm_mul_ps(m, c.m));
#else
	return Color(a * c.a, r * c.r, g * c.g, b * c.b);
#endif
}

const Color Color::Lerp(const Color& a, const Color& b, float t)
{
	Color color;
#if _MATH_SIMD_INTRINSIC_
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


const Color Color::Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2)
{
#if _MATH_SIMD_INTRINSIC_
	static __m128 _ps1 = _mm_set1_ps(1.f);
	__m128 mt1 = _mm_set1_ps(t1);
	__m128 mt2 = _mm_set1_ps(t2);
	__m128 mrt1 = _mm_sub_ps(_ps1, mt1);
	__m128 mrt2 = _mm_sub_ps(_ps1, mt2);
	__m128 tmp1 = _mm_add_ps(_mm_mul_ps(a.m, mrt1), _mm_mul_ps(b.m, mt1));
	__m128 tmp2 = _mm_add_ps(_mm_mul_ps(c.m, mrt1), _mm_mul_ps(d.m, mt1));

	Color color;
	color.m = _mm_add_ps(_mm_mul_ps(tmp1, mrt2), _mm_mul_ps(tmp2, mt2));
	return color;
#else
	return Color::Lerp(Color::Lerp(a, b, t1), Color::Lerp(c, d, t1), t2);
#endif
}

Color::operator Color32() const
{
    Color32 color;
#if _MATH_SIMD_INTRINSIC_
    static __m128 _ps255 = _mm_set1_ps(255.f);
    __m128 tmp = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m));
    __m128i im = _mm_cvtps_epi32(_mm_mul_ps(tmp, _ps255));
    color.r = _mm_extract_epi8(im, 0);
    color.g = _mm_extract_epi8(im, 4);
    color.b = _mm_extract_epi8(im, 8);
    color.a = _mm_extract_epi8(im, 12);
#else
    color.a = (u8)(Mathf::Clamp01(a) * 255);
	color.r = (u8)(Mathf::Clamp01(r) * 255);
	color.g = (u8)(Mathf::Clamp01(g) * 255);
	color.b = (u8)(Mathf::Clamp01(b) * 255);
#endif
    return color;
}

Color32::operator Color() const
{
	Color color;
#if _MATH_SIMD_INTRINSIC_
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