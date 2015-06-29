#ifndef _RASTERIZER_SHADER_FUNCTION_H_
#define _RASTERIZER_SHADER_FUNCTION_H_

#include "base/header.h"
#include "base/color.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "shader.hpp"

namespace rasterizer
{

struct ShaderF
{
	static const Color LightingLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
	{
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
		Color output;
		output.rgb = input.ambient.rgb + input.diffuse.rgb * lightColor.rgb * (nDotL * attenuation * 2.f);
		output.a = input.diffuse.a;
		return output;
	}

	static const Color LightingHalfLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
	{
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
		nDotL = nDotL * 0.8f + 0.2f;
		Color output;
		output.rgb = input.ambient.rgb + input.diffuse.rgb * lightColor.rgb * (nDotL * attenuation * 2.f);
		output.a = input.diffuse.a;
		return output;
	}

	static const Color LightingBlinnPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation)
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

	static const Color LightingPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation)
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

};

} // namespace rasterizer

#endif //! _RASTERIZER_SHADER_FUNCTION_H_