#ifndef _BASE_COLOR_H_
#define _BASE_COLOR_H_

#include "header.h"

namespace rasterizer
{

struct Color32;
struct Color
{
	float a, r, g, b;
	Color(float _a, float _r, float _g, float _b);
	Color(const Color& _color);
	Color(const Color32& _color32);

	const Color Add(const Color& c) const;
	const Color Multiply(float s) const;
	const Color Modulate(const Color& c) const;

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

	Color32(u32 _argb);
	Color32(u32 _a, u32 _r, u32 _g, u32 _b);
	Color32(const Color32& _color32);
	Color32(const Color& _color);

	static const Color32 white;
	static const Color32 black;
};

}
#endif //!_BASE_COLOR_H_
