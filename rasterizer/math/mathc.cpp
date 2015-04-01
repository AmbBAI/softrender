#include "mathf.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

namespace rasterizer {
    
const float Mathf::epsilon = FLT_EPSILON;
const float Mathf::inifinity = FLT_MAX;
const float Mathf::PI = (float)M_PI;
const float Mathf::deg2rad = (float)M_PI / 180.0f;
const float Mathf::rad2deg = 180.0f / (float)M_PI;

const Vector2 Vector2::zero = Vector2(0.f, 0.f);
const Vector2 Vector2::one = Vector2(1.f, 1.f);
    
const Vector3 Vector3::zero = Vector3(0.f, 0.f, 0.f);
const Vector3 Vector3::one = Vector3(1.f, 1.f, 1.f);
const Vector3 Vector3::front = Vector3(0.f, 0.f, -1.f);
const Vector3 Vector3::up = Vector3(0.f, 1.f, 0.f);
const Vector3 Vector3::right = Vector3(1.f, 0.f, 0.f);
const Vector3 Vector3::back = Vector3(0.f, 0.f, 1.f);
const Vector3 Vector3::down = Vector3(0.f, -1.f, 0.f);
const Vector3 Vector3::left = Vector3(-1.f, 0.f, 0.f);

}