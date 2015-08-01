#ifndef _BASE_COLOR_H_
#define _BASE_COLOR_H_

#include "header.h"
#include "math/mathf.h"
#include "math/vector3.h"
#include "math/vector4.h"

namespace rasterizer
{

struct ColorRGB
{
	float r, g, b;

	ColorRGB() = default;
	ColorRGB(float _r, float _g, float _b)
		: r(_r), g(_g), b(_b) {}
	//: a(Mathf::Clamp01(_a))
	//, r(Mathf::Clamp01(_r))
	//, g(Mathf::Clamp01(_g))
	//{}
	ColorRGB(const Vector3& rgb)
		: ColorRGB(rgb.x, rgb.y, rgb.z) {}

	inline const ColorRGB operator +(const ColorRGB& color) const;
	inline const ColorRGB operator +=(const ColorRGB& color);
	inline const ColorRGB operator *(float f) const;
	inline const ColorRGB operator *(const ColorRGB& color) const;
	inline const ColorRGB operator *=(float f);
	inline const ColorRGB operator *=(const ColorRGB& color);

	inline operator Vector3() const;
};

struct Color32;
struct Color
{
	union
	{
		struct
		{
            union
            {
                struct { float r, g, b; };
				ColorRGB rgb;
            };
            float a;
		};
#if _NOCRASH_
		__m128 m;
#endif
	};
	
	Color() = default;
	Color(float _a, float _r, float _g, float _b)
#if _NOCRASH_
		: Color(_mm_setr_ps(_r, _g, _b, _a)) {}
#else
		: a(_a), r(_r), g(_g), b(_b) {}
	//: a(Mathf::Clamp01(_a))
	//, r(Mathf::Clamp01(_r))
	//, g(Mathf::Clamp01(_g))
	//, b(Mathf::Clamp01(_b))
	//{}
#endif
    
	Color(const Vector4& rgba)
		: Color(rgba.w, rgba.x, rgba.y, rgba.z) {}

#if _NOCRASH_
	Color(__m128 _m)
		: m(_m) {}
    //{
    //    m = _mm_min_ps(_mm_set1_ps(1.f), _mm_max_ps(_mm_set1_ps(0.f), m));
    //}
#endif

	inline const Color operator +(const Color& color) const;
	inline const Color operator +=(const Color& color);
	inline const Color operator *(float f) const;
	inline const Color operator *(const Color& color) const;
	inline const Color operator *=(float f);
	inline const Color operator *=(const Color& color);

	inline Color Clamp() const;
	inline Color Inverse() const;

	static inline const Color Lerp(const Color& a, const Color& b, float t);
	static inline const Color Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2);

	inline operator Color32() const;
	inline operator Vector4() const;

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
		uint32_t rgba;
		struct 
		{
			uint8_t r, g, b, a;
		};
	};

	Color32() = default;
	Color32(uint32_t _rgba)
        : rgba(_rgba) {}    
	Color32(uint8_t _a, uint8_t _r, uint8_t _g, uint8_t _b)
        : a(_a), r(_r), g(_g), b(_b){}

	inline operator Color() const;

	static const Color32 white;
	static const Color32 black;
};

} // namespace rasterizer

#include "color.inl"

#endif //!_BASE_COLOR_H_
