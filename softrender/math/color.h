#ifndef _MATH_COLOR_H_
#define _MATH_COLOR_H_

#include "base/header.h"
#include "math/mathf.h"
#include "math/vector3.h"
#include "math/vector4.h"

namespace sr
{

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
				Vector3 rgb;
			};
			float a;
		};
	};

	Color() = default;
	Color(float _a, float _r, float _g, float _b)
		: a(_a), r(_r), g(_g), b(_b) {}

	Color(const Vector3& rgb, float a)
		: Color(a, rgb.x, rgb.y, rgb.z) {}

	Color(const Vector4& rgba)
		: Color(rgba.w, rgba.x, rgba.y, rgba.z) {}

	inline Color operator +(const Color& color) const;
	inline Color operator +=(const Color& color);
	inline Color operator +(float f) const;
	inline Color operator +=(float f);
	inline Color operator *(float f) const;
	inline Color operator *(const Color& color) const;
	inline Color operator *=(float f);
	inline Color operator *=(const Color& color);

	inline Color Clamp() const;

	static inline Color Lerp(const Color& a, const Color& b, float t);
	static inline Color Lerp(const Color& a, const Color& b, const Color& c, const Color& d, float t1, float t2);

	static inline Color GammaToLinearSpace(const Color& c);
	static inline Color GammaToLinearSpaceFast(const Color& c);
	static inline Color LinearToGammaSpace(const Color& c);
	static inline Color LinearToGammaSpaceFast(const Color& c);

	static inline float GammaToLinearSpaceExact(float value);
	static inline float LinearToGammaSpaceExact(float value);

	inline operator Color32() const;
	inline operator Vector4() const;

	static const Color clear;
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
		: a(_a), r(_r), g(_g), b(_b) {}

	inline operator Color() const;

	static const Color32 white;
	static const Color32 black;
};

} // namespace sr

#include "color.inl"

#endif //!_MATH_COLOR_H_
