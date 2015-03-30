#include "color.h"
#include "math/mathf.h"

namespace rasterizer
{

const Color Color::white = Color(1.f, 1.f, 1.f, 1.f);
const Color Color::black = Color(1.f, 0.f, 0.f, 0.f);
const Color Color::red = Color(1.f, 1.f, 0.f, 0.f);
const Color Color::green = Color(1.f, 0.f, 1.f, 0.f);
const Color Color::blue = Color(1.f, 0.f, 0.f, 1.f);

Color::Color(float _a, float _r, float _g, float _b)
#if _MATH_SIMD_INTRINSIC_
    : Color(_mm_setr_ps(_a, _r, _g, _b))
    {
    }
#else
    : a(Mathf::Clamp01(_a))
    , r(Mathf::Clamp01(_r))
    , g(Mathf::Clamp01(_g))
    , b(Mathf::Clamp01(_b))
    {}
#endif

#if _MATH_SIMD_INTRINSIC_
Color::Color(__m128 _m)
    : m(_m)
{
    m = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m));
}
#endif
    
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
    t = _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(1.f), _mm_max_ss(_mm_set_ss(0.f), _mm_set_ss(t))));
	__m128 t2 = _mm_set1_ps(t);
	__m128 t1 = _mm_sub_ps(_mm_set_ps1(1.f), t2);
	color.m = _mm_add_ps(_mm_mul_ps(a.m, t1), _mm_mul_ps(b.m, t2));
#else
    t = Mathf::Clamp01(t);
	color.a = Mathf::Lerp(a.a, b.a, t);
	color.r = Mathf::Lerp(a.r, b.r, t);
	color.g = Mathf::Lerp(a.g, b.g, t);
	color.b = Mathf::Lerp(a.b, b.b, t);
#endif
	return color;
}

Color::operator Color32() const
{
    Color32 color;
#if _MATH_SIMD_INTRINSIC_
    __m128 tmp = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m));
    __m128i im = _mm_cvtps_epi32(_mm_mul_ps(tmp, _mm_set1_ps(255.f)));
    color.a = _mm_extract_epi8(im, 0);
    color.r = _mm_extract_epi8(im, 4);
    color.g = _mm_extract_epi8(im, 8);
    color.b = _mm_extract_epi8(im, 12);
#else
	color.a = (u8)(a * 255);
	color.r = (u8)(r * 255);
	color.g = (u8)(g * 255);
	color.b = (u8)(b * 255);
#endif
    return color;
}

const Color32 Color32::white = Color32(0xffffffff);
const Color32 Color32::black = Color32(0xff000000);

Color32::Color32(u32 _argb)
	: rgba(_argb)
{
}

Color32::Color32(u8 _a, u8 _r, u8 _g, u8 _b)
	: a(_a)
	, r(_r)
	, g(_g)
	, b(_b)
{
}

Color32::operator Color() const
{
	Color color;
#if _MATH_SIMD_INTRINSIC_
    __m128 m = _mm_cvtepi32_ps(_mm_cvtepu8_epi32((__m128i&)rgba));
    color.m = _mm_mul_ps(m, _mm_set1_ps(1.f / 255.f));
#else
    color.a = a / 255.f;
    color.r = r / 255.f;
    color.g = g / 255.f;
    color.b = b / 255.f;
#endif
    return color;
}

}