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
	: a(Mathf::Clamp01(_a))
	, r(Mathf::Clamp01(_r))
	, g(Mathf::Clamp01(_g))
	, b(Mathf::Clamp01(_b))
{
}

Color::Color(const Color& _color)
	: a(_color.a)
	, r(_color.r)
	, g(_color.g)
	, b(_color.b)
{
}

Color::Color(const Color32& _color32)
	: a(_color32.a / 255.f)
	, r(_color32.r / 255.f)
	, g(_color32.g / 255.f)
	, b(_color32.b / 255.f)
{
}

const Color Color::Add(const Color& c) const
{
	return Color(a + c.a, r + c.r, g + c.g, b + c.b);
}

const Color Color::Multiply(float s) const
{
	return Color(a * s, r * s, g * s, b * s);
}

const Color Color::Modulate(const Color& c) const
{
	return Color(a * c.a, r * c.r, g * c.g, b * c.b);
}

const Color Color::Lerp(const Color& a, const Color& b, float t)
{
	t = Mathf::Clamp01(t);
	Color color = Color();
	color.a = a.a * (1 - t) + b.a * t;
	color.r = a.r * (1 - t) + b.r * t;
	color.g = a.g * (1 - t) + b.g * t;
	color.b = a.b * (1 - t) + b.b * t;
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
	//printf("%x %x %x %x %x - %d\n", a, r, g, b, argb, sizeof(Color));
}

Color32::Color32(const Color32& _color32)
	: rgba(_color32.rgba)
{
}

Color32::Color32(const Color& _color)
	: a((u8)(_color.a * 255))
	, r((u8)(_color.r * 255))
	, g((u8)(_color.g * 255))
	, b((u8)(_color.b * 255))
{
}

}