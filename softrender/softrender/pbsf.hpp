#ifndef _SOFTRENDER_PBSF_H_
#define _SOFTRENDER_PBSF_H_

#include "base/header.h"
#include "math/color.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"

namespace sr
{

struct PBSInput
{
	Vector3 albedo;
	Vector3 normal;
	float roughness;
	float specular;
};

struct PBSF
{
	static float GGXTerm(float nDotH, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float d = (nDotH * nDotH) * (a2 - 1.f) + 1.f;
		return a2 / (Mathf::PI * d * d);
	}

	static float VisibilityTerm(float nDotL, float nDotV, float roughness)
	{
		float k = (roughness + 1.f) * (roughness + 1.f) / 8.f;
		float gl = nDotL / (nDotL * (1.f - k) + k);
		float gv = nDotV / (nDotV * (1.f - k) + k);
		return gl * gv;
	}

	static float SmithVisibilityTerm(float nDotL, float nDotV, float k)
	{
		float gl = nDotL * (1 - k) + k;
		float gv = nDotV * (1 - k) + k;
		return 1.f / (gl * gv + 1e-5f);
	}

	static float FresnelTerm(float vDotH, float specular)
	{
		return specular + (1 - specular) * Mathf::Pow(1.f - vDotH, 5.f);
	}

	static float DisneyDiffuseTerm(float nDotL, float nDotV, float lDotH, float roughness)
	{
		float nlPow5 = Mathf::Pow(1.f - nDotL, 5.f);
		float nvPow5 = Mathf::Pow(1.f - nDotV, 5.f);
		float fD90 = 0.5f + 2.f * lDotH * lDotH * roughness;
		return (1.f + (fD90 - 1.f) * nlPow5) * (1.f + (fD90 - 1.f) * nvPow5) / Mathf::PI;
	}

	static ColorRGB BRDF(const PBSInput& input, const Vector3& lightDir, const Vector3& viewDir)
	{
		Vector3 floatDir = (lightDir + viewDir).Normalize();
		float nDotL = input.normal.Dot(lightDir);
		float nDotV = input.normal.Dot(viewDir);
		float nDotH = input.normal.Dot(floatDir);
		float vDotH = viewDir.Dot(floatDir);
		float lDotH = lightDir.Dot(floatDir);

		float diffuseTerm = DisneyDiffuseTerm(nDotL, nDotV, lDotH, input.roughness);
		float D = GGXTerm(nDotH, input.roughness);
		float V = SmithVisibilityTerm(nDotL, nDotV, input.roughness);
		float F = FresnelTerm(vDotH, input.specular);
		//return input.albedo * diffuseTerm + Color::white.rgb * (D * V * F / (4.f * nDotL * nDotV));
		return input.albedo * diffuseTerm;
		//return Color::white.rgb * D;
		//return Color::white.rgb * V;
		//return Color::white.rgb * F;
		//return Color::white.rgb * (1.f / (4.f * nDotL * nDotV));
	}

	static ColorRGB BRDF2(const PBSInput& input, const Vector3& lightDir, const Vector3& viewDir)
	{
		Vector3 floatDir = (lightDir + viewDir).Normalize();
		float nDotL = Mathf::Clamp01(input.normal.Dot(lightDir));
		float lDotH = Mathf::Clamp01(lightDir.Dot(floatDir));
		float nDotH = Mathf::Clamp01(input.normal.Dot(floatDir));

		float D = GGXTerm(nDotH, input.roughness);
		float VF = 1.f / (4.f * lDotH * lDotH * (input.roughness + 0.5f));
		return input.albedo * nDotL + Color::white.rgb * (D * VF * nDotL * input.specular);
	}
};

} // namespace sr

#endif //! _SOFTRENDER_PBSF_H_
