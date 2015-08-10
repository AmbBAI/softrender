#include "mathf.h"
#include "color.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

namespace sr {
    
const float Mathf::epsilon = FLT_EPSILON;
const float Mathf::inifinity = FLT_MAX;
const float Mathf::PI = (float)M_PI;
const float Mathf::deg2rad = (float)M_PI / 180.0f;
const float Mathf::rad2deg = 180.0f / (float)M_PI;

const Color Color::white = Color(1.f, 1.f, 1.f, 1.f);
const Color Color::black = Color(1.f, 0.f, 0.f, 0.f);
const Color Color::red = Color(1.f, 1.f, 0.f, 0.f);
const Color Color::green = Color(1.f, 0.f, 1.f, 0.f);
const Color Color::blue = Color(1.f, 0.f, 0.f, 1.f);

const Color32 Color32::white = Color32(0xffffffff);
const Color32 Color32::black = Color32(0xff000000);

const Vector2 Vector2::zero = Vector2(0.f, 0.f);
const Vector2 Vector2::one = Vector2(1.f, 1.f);
    
const Vector3 Vector3::zero = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::one = Vector3(1.f, 1.f, 1.f);
const Vector3 Vector3::front = Vector3(0.f, 0.f, 1.f);
const Vector3 Vector3::up = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::right = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::back = Vector3(0.f, 0.f, -1.f);
const Vector3 Vector3::down = Vector3(0.f, -1.f, 0.f);
const Vector3 Vector3::left = Vector3(-1.f, 0.f, 0.f);


}