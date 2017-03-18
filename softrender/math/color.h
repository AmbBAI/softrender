#ifndef _MATH_COLOR_H_
#define _MATH_COLOR_H_

#include "base/header.h"
#include "math/mathf.h"
#include "math/vector3.h"
#include "math/vector4.h"

namespace sr
{

struct ColorRGB
{
	float r, g, b;

	ColorRGB() = default;
	ColorRGB(float _r, float _g, float _b)
		: r(_r), g(_g), b(_b) {}
	ColorRGB(const Vector3& rgb)
		: ColorRGB(rgb.x, rgb.y, rgb.z) {}

	inline ColorRGB operator +(const ColorRGB& color) const;
	inline ColorRGB operator +=(const ColorRGB& color);
	inline ColorRGB operator *(float f) const;
	inline ColorRGB operator *(const ColorRGB& color) const;
	inline ColorRGB operator *=(float f);
	inline ColorRGB operator *=(const ColorRGB& color);

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
#endif
    
	Color(const Vector4& rgba)
		: Color(rgba.w, rgba.x, rgba.y, rgba.z) {}

#if _NOCRASH_
	Color(__m128 _m)
		: m(_m) {}
#endif

	inline Color operator +(const Color& color) const;
	inline Color operator +=(const Color& color);
	inline Color operator *(float f) const;
	inline Color operator *(const Color& color) const;
	inline Color operator *=(float f);
	inline Color operator *=(const Color& color);

	inline Color Clamp() const;
	inline Color Inverse() const;

	static inline Color Lerp(const Color& a, const Color& b, float t);
	static inline Color Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2);

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

} // namespace sr

#include "color.inl"

#endif //!_MATH_COLOR_H_
