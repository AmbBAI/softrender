#ifndef _BASE_COLOR_H_
#define _BASE_COLOR_H_

#include "header.h"

namespace rasterizer
{

struct Color32;
struct Color
{
	union
	{
		struct
		{
			float r, g, b, a;
		};
#if _MATH_SIMD_INTRINSIC_
		__m128 m;
#endif
	};
	
	Color() = default;
	Color(float _a, float _r, float _g, float _b)
#if _MATH_SIMD_INTRINSIC_
    : Color(_mm_setr_ps(_r, _g, _b, _a))
    {}
#else
    : a(Mathf::Clamp01(_a))
    , r(Mathf::Clamp01(_r))
    , g(Mathf::Clamp01(_g))
    , b(Mathf::Clamp01(_b))
    {}
#endif
    
#if _MATH_SIMD_INTRINSIC_
    Color(__m128 _m)
        : m(_m)
    {
        m = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m));
    }
#endif

	inline const Color Add(const Color& c) const;
	inline const Color Multiply(float s) const;
	inline const Color Modulate(const Color& c) const;
	static inline const Color Lerp(const Color& a, const Color& b, float t);
	static inline const Color Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2);

	inline operator Color32() const;

	static const Color white;
	static const Color black;
	static const Color red;
	static const Color green;
	static const Color blue;
};


struct Color32
{
	union
	{
		u32 rgba;
		struct 
		{
			u8 r, g, b, a;
		};
	};

	Color32() = default;
	Color32(u32 _rgba)
        : rgba(_rgba) {}    
	Color32(u8 _a, u8 _r, u8 _g, u8 _b)
        : a(_a), r(_r), g(_g), b(_b){}

	inline operator Color() const;

	static const Color32 white;
	static const Color32 black;
};


} // namespace rasterizer

#include "color.inl"

#endif //!_BASE_COLOR_H_
