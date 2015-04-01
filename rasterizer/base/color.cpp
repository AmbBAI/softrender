#include "color.h"
#include "math/mathf.h"

namespace rasterizer
{

const Color Color::white = Color(1.f, 1.f, 1.f, 1.f);
const Color Color::black = Color(1.f, 0.f, 0.f, 0.f);
const Color Color::red = Color(1.f, 1.f, 0.f, 0.f);
const Color Color::green = Color(1.f, 0.f, 1.f, 0.f);
const Color Color::blue = Color(1.f, 0.f, 0.f, 1.f);
 
const Color32 Color32::white = Color32(0xffffffff);
const Color32 Color32::black = Color32(0xff000000);

}