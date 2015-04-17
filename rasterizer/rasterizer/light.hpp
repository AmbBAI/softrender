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
    float falloff = 1.f;
    float theta = 60.f;
    float phi = 30.f;
    
    float cosHalfTheta;
    float cosHalfPhi;
    
    void Initilize()
    {
        if (type == LightType_Spot)
        {
            cosHalfPhi = Mathf::Cos(phi * Mathf::deg2rad / 2.f);
            cosHalfTheta = Mathf::Cos(theta * Mathf::deg2rad / 2.f);
        }
    }
	
#if _MATH_SIMD_INTRINSIC_ && defined(_MSC_VER)
	MEMALIGN_NEW_OPERATOR_OVERRIDE(16)
	MEMALIGN_DELETE_OPERATOR_OVERRIDE
#endif
};

}

#endif //!_RASTERIZER_LIGHT_HPP_
