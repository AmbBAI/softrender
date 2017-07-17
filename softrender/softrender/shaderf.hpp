#ifndef _SOFTRENDER_SHADERF_H_
#define _SOFTRENDER_SHADERF_H_

#include "base/header.h"
#include "math/color.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace sr
{

struct LightInput
{
	Color ambient;
	Color diffuse;
	Color specular;
	float shininess;
};

struct ShaderF
{
	static Vector3 LightingLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Vector3& lightColor, float attenuation)
	{
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
		return input.ambient.rgb * input.diffuse.rgb + input.diffuse.rgb * lightColor * (nDotL * attenuation);
	}

	static Vector3 LightingHalfLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Vector3& lightColor, float attenuation)
	{
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
		nDotL = nDotL * 0.8f + 0.2f;
		return input.ambient.rgb * input.diffuse.rgb + input.diffuse.rgb * lightColor * (nDotL * attenuation);
	}

	static Vector3 LightingBlinnPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Vector3& lightColor, const Vector3& viewDir, float attenuation)
	{
		float lambertian = Mathf::Clamp01(normal.Dot(lightDir));
		float specular = 0.f;

		if (lambertian > 0.f)
		{
			Vector3 halfDir = (lightDir + viewDir).Normalize();
			float specAngle = Mathf::Max(halfDir.Dot(normal), 0.f);
			specular = Mathf::Pow(specAngle, input.shininess * 4.f);
		}

		return input.ambient.rgb * input.diffuse.rgb
			+ input.diffuse.rgb * lightColor * (lambertian * attenuation)
			+ input.specular.rgb * lightColor * (specular * attenuation);
	}

	static Vector3 LightingPhong(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Vector3& lightColor, const Vector3& viewDir, float attenuation)
	{
		float lambertian = Mathf::Clamp01(normal.Dot(lightDir));
		float specular = 0.f;

		if (lambertian > 0.f)
		{
			Vector3 reflectDir = (normal * normal.Dot(lightDir) * 2.f - lightDir).Normalize();
			float specAngle = Mathf::Max(reflectDir.Dot(viewDir), 0.f);
			specular = Mathf::Pow(specAngle, input.shininess);
		}

		return input.ambient.rgb * input.diffuse.rgb
			+ input.diffuse.rgb * lightColor * (lambertian * attenuation)
			+ input.specular.rgb * lightColor * (specular * attenuation);
	}

};

} // namespace sr

#endif //! _SOFTRENDER_SHADERF_H_
