#ifndef _RASTERIZER_SHADER_FUNCTION_H_
#define _RASTERIZER_SHADER_FUNCTION_H_

#include "base/header.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace rasterizer
{
namespace shader
{
	struct LightInput
	{
		Color ambient;
		Color diffuse;
		Color specular;
		float shininess;
	};

	Vector3 GetViewDir(const Vector3 position)
	{
		assert(renderData != nullptr);
		assert(Rasterizer::camera != nullptr);

		return (Rasterizer::camera->GetPosition() - position).Normalize();
	}

	typedef Color(*LightFunc)(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation);
	static Color CalcLight(const LightInput& input, const LightPtr& light, LightFunc lightFunc, const Vector3& position, const Vector3& normal)
	{
		if (light && lightFunc)
		{
			Vector3 lightDir = light->direction.Negate();
			float attenuation = 1.f;
			float intensity = light->intensity;
			switch (light->type) {
			case Light::LightType_Directional:
				break;
			case Light::LightType_Point:
			{
				lightDir = light->position - position;
				float distance = lightDir.Length();
				lightDir /= distance;
				attenuation = light->range * light->CalcAttenuation(distance);
			}
			break;
			case Light::LightType_Spot:
			{
				lightDir = light->position - position;
				float distance = lightDir.Length();
				lightDir /= distance;
				attenuation = light->range * light->CalcAttenuation(distance);
				intensity = intensity * light->CalcSpotlightFactor(lightDir);
			}
			break;
			}
			Color lightColor = light->color;
			lightColor.rgb = lightColor.rgb * intensity;
			Vector3 viewDir = GetViewDir(position);
			return lightFunc(input, normal, lightDir, lightColor, viewDir, attenuation);
		}
		else
		{
			Color out = input.diffuse;
			out.rgb += input.ambient.rgb;
			return out;
		}
	}


	const Color LightingLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
	{
		float nDotL = Mathf::Clamp01(normal.Dot(lightDir));
		Color output;
		output.rgb = input.ambient.rgb + input.diffuse.rgb * lightColor.rgb * (nDotL * attenuation * 2.f);
		output.a = input.diffuse.a;
		return output;
	}

	const Color LightingHalfLambert(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, float attenuation)
	{
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

} // namespace shader

} // namespace rasterizer

#endif //! _RASTERIZER_SHADER_FUNCTION_H_