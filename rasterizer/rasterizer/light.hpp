#ifndef _RASTERIZER_LIGHT_HPP_
#define _RASTERIZER_LIGHT_HPP_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"

namespace rasterizer
{

struct Light;
typedef std::shared_ptr<Light> LightPtr;

struct Light
{
    enum LightType
    {
        LightType_Directional,
        LightType_Point,
        LightType_Spot
    };
    LightType type = LightType_Directional;
    Color color = Color(1.f, 1.f, 1.f, 1.f);
    float intensity = 0.5f;
    
    Vector3 position = Vector3(0.f, 0.f, 0.f);
    Vector3 direction = Vector3(0.f, -1.f, 0.f);
    float range = 10.f;
};

}

#endif //!_RASTERIZER_LIGHT_HPP_
