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
	: a(Mathf::Clamp01(_color32.a / 255.f))
	, r(Mathf::Clamp01(_color32.r / 255.f))
	, g(Mathf::Clamp01(_color32.g / 255.f))
	, b(Mathf::Clamp01(_color32.b / 255.f))
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


const Color32 Color32::white = Color32(0xffffffff);
const Color32 Color32::black = Color32(0xff000000);

Color32::Color32(u32 _argb)
	: rgba(_argb)
{
}

Color32::Color32(u32 _a, u32 _r, u32 _g, u32 _b)
	: a(Mathf::Clamp(_a, 0, 255))
	, r(Mathf::Clamp(_r, 0, 255))
	, g(Mathf::Clamp(_g, 0, 255))
	, b(Mathf::Clamp(_b, 0, 255))
{
	//printf("%x %x %x %x %x - %d\n", a, r, g, b, argb, sizeof(Color));
}

Color32::Color32(const Color32& _color32)
	: rgba(_color32.rgba)
{
}

Color32::Color32(const Color& _color)
	: a((u8)(Mathf::Clamp((u32)(_color.a * 255), 0, 255)))
	, r((u8)(Mathf::Clamp((u32)(_color.r * 255), 0, 255)))
	, g((u8)(Mathf::Clamp((u32)(_color.g * 255), 0, 255)))
	, b((u8)(Mathf::Clamp((u32)(_color.b * 255), 0, 255)))
{
}

rasterizer::Color32 Color32::Lerp(const Color32& a, const Color32& b, float t)
{
	t = Mathf::Clamp01(t);
	return Color32(
		255,
		a.r * (1 - t) + b.r * t,
		a.g * (1 - t) + b.g * t,
		a.b * (1 - t) + b.b * t
		);
}

}