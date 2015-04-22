#ifndef _RASTERIZER_SHADER_H_
#define _RASTERIZER_SHADER_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace rasterizer
{

struct LightInput
{
    Color ambient;
    Color diffuse;
    Color specular;
	float shininess;
};

    
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
    CameraPtr camera = nullptr;

	virtual void VertexShader(VSOutputType& out) = 0;
	virtual const Color PixelShader(const PSInputType& input) = 0;
    
    const Color LightingLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
    {
        assert(light != nullptr);
        float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
        Color output;
        output.rgb = input.ambient.rgb + input.diffuse.rgb * lightColor.rgb * (nDotL * attenuation * 2.f);
        output.a = input.diffuse.a;
        return output;
    }
    
    const Color LightingHalfLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
    {
        assert(light != nullptr);
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
        nDotL = nDotL * 0.8f + 0.2f;
        Color output;
        output.rgb = input.ambient.rgb + input.diffuse.rgb * lightColor.rgb * (nDotL * attenuation * 2.f);
        output.a = input.diffuse.a;
        return output;
    }
    
    const Color LightingBlinnPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation)
    {
		float lambertian = Mathf::Clamp01(normal.Dot(lightDir));
        float specular = 0.f;
        
        if (lambertian > 0.f)
        {
            Vector3 halfDir = (lightDir + viewDir).Normalize();
            float specAngle = Mathf::Max(halfDir.Dot(normal), 0.f);
            specular = Mathf::Pow(specAngle, input.shininess * 4.f);
        }
        
        Color output = Color::white;
		output.rgb = input.ambient.rgb
			+ (input.diffuse.rgb * lightColor.rgb * lambertian
			+ input.specular.rgb * lightColor.rgb * specular) * attenuation;
        output.a = input.diffuse.a;
        return output;
    }
    
    const Color LightingPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation)
    {
		float lambertian = Mathf::Clamp01(normal.Dot(lightDir));
        float specular = 0.f;
        
        if (lambertian > 0.f)
        {
            Vector3 reflectDir = (normal * normal.Dot(lightDir) * 2.f - lightDir).Normalize();
            float specAngle = Mathf::Max(reflectDir.Dot(viewDir), 0.f);
			specular = Mathf::Pow(specAngle, input.shininess);
        }
        
        Color output;
        output.rgb = input.ambient.rgb
            + (input.diffuse.rgb * lightColor.rgb * lambertian
            + input.specular.rgb * lightColor.rgb * specular) * attenuation;
        output.a = input.diffuse.a;
        return output;
    }
    
	//TODO Phong, BRDF
};

}

#endif //! _RASTERIZER_SHADER_Hv_