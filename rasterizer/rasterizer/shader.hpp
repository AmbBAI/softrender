#ifndef _RASTERIZER_SHADER_H_
#define _RASTERIZER_SHADER_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace rasterizer
{

template<typename VSOutputType, typename PSInputType>
struct Shader
{
	Matrix4x4 _MATRIX_MVP;
    Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
    Matrix4x4 _MATRIX_P;
    Matrix4x4 _MATRIX_VP;
    Matrix4x4 _Object2World;
    Matrix4x4 _World2Object;
    MaterialPtr material = nullptr;
    LightPtr light = nullptr;

	virtual void VertexShader(VSOutputType& out) = 0;
	virtual const Color PixelShader(const PSInputType& input) = 0;
    
    const Color LightingLambert(const Color& color, const Vector3& normal, const Vector3& lightDir, float attenuation)
    {
        assert(light != nullptr);
        float nDotL = Mathf::Max(normal.Dot(lightDir.Negate()), 0.f);
        Color o;
        o.rgb = color.rgb * light->color.rgb * (nDotL * attenuation * 2.f);
        o.a = color.a;
        return o;
    }
    
    const Color LightingHalfLambert(const Color& color, const Vector3& normal, const Vector3& lightDir, float attenuation)
    {
        assert(light != nullptr);
        float nDotL = Mathf::Max(normal.Dot(lightDir.Negate()), 0.f);
        nDotL = nDotL * 0.8f + 0.2f;
        Color o;
        o.rgb = color.rgb * light->color.rgb * (nDotL * attenuation * 2.f);
        o.a = color.a;
        return o;
    }
};

}

#endif //! _RASTERIZER_SHADER_H_