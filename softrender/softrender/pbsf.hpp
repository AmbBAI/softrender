#ifndef _SOFTRENDER_PBSF_H_
#define _SOFTRENDER_PBSF_H_

#include "base/header.h"
#include "math/color.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "shader.hpp"

namespace sr
{

struct PBSInput
{
	Vector3 albedo;
	Vector3 normal;
	Vector3 specColor;
	float roughness;
	float specular;
	CubemapPtr envMap;
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

	// Generic Smith-Schlick visibility term
	static float SmithVisibilityTerm(float NdotL, float NdotV, float k)
	{
		float gL = NdotL * (1.f - k) + k;
		float gV = NdotV * (1.f - k) + k;
		return 1.f / (gL * gV + 1e-5f);
	}

	// Smith-Schlick derived for Beckmann
	static float SmithBeckmannVisibilityTerm(float NdotL, float NdotV, float roughness)
	{
		float c = 0.797884560802865f; // c = sqrt(2 / Pi)
		float k = roughness * roughness * c;
		return SmithVisibilityTerm(NdotL, NdotV, k) * 0.25f;
	}

	// Smith-Schlick derived for GGX
	static float SmithGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
	{
		float k = roughness * roughness * 0.5f;
		return SmithVisibilityTerm(NdotL, NdotV, k) * 0.25f;
	}

	// Unity VisiblityTerm for GGX
	static float SmithJointGGXVisibilityTerm(float nDotL, float nDotV, float roughness)
	{
		float a = roughness * roughness;
		float lambdaV = nDotL * (nDotV * (1.f - a) + a);
		float lambdaL = nDotV * (nDotL * (1.f - a) + a);
		return 0.5f / (lambdaV + lambdaL + 1e-5f);
	}

	static float FresnelTerm(float vDotH, float specular)
	{
		return specular + (1 - specular) * Mathf::Pow(1.f - vDotH, 5.f);
	}

	static Vector3 FresnelTerm(float vDotH, Vector3 specular)
	{
		return specular + (Vector3::one - specular) * Mathf::Pow(1.f - vDotH, 5.f);
	}

	static float DisneyDiffuseTerm(float nDotL, float nDotV, float lDotH, float roughness)
	{
		float nlPow5 = Mathf::Pow(1.f - nDotL, 5.f);
		float nvPow5 = Mathf::Pow(1.f - nDotV, 5.f);
		float fD90 = 0.5f + 2.f * lDotH * lDotH * roughness;
		return Mathf::Clamp01((1.f + (fD90 - 1.f) * nlPow5) * (1.f + (fD90 - 1.f) * nvPow5));
	}

	static ColorRGB BRDF(const PBSInput& input, const Vector3& lightDir, const Vector3& viewDir)
	{
		Vector3 halfdir = (lightDir + viewDir).Normalize();
		float nDotL = Mathf::Clamp01(input.normal.Dot(lightDir));
		float nDotV = Mathf::Clamp01(input.normal.Dot(viewDir));
		float nDotH = Mathf::Clamp01(input.normal.Dot(halfdir));
		float vDotH = Mathf::Clamp01(viewDir.Dot(halfdir));
		float lDotH = Mathf::Clamp01(lightDir.Dot(halfdir));

		float diffuseTerm = DisneyDiffuseTerm(nDotL, nDotV, lDotH, input.roughness);
		float D = GGXTerm(nDotH, input.roughness);
		float V = SmithGGXVisibilityTerm(nDotL, nDotV, input.roughness);
		float F = FresnelTerm(vDotH, input.specular);
		return input.albedo * (diffuseTerm * nDotL) + input.specColor * (D * V * F * nDotL * Mathf::PI);
		//return Color::white.rgb * (diffuseTerm * nDotL);
		//return Color::white.rgb * D;
		//return Color::white.rgb * V;
		//return Color::white.rgb * F;
	}

	static ColorRGB BRDF2(const PBSInput& input, const Vector3& lightDir, const Vector3& viewDir)
	{
		Vector3 floatDir = (lightDir + viewDir).Normalize();
		float nDotL = Mathf::Clamp01(input.normal.Dot(lightDir));
		float lDotH = Mathf::Clamp01(lightDir.Dot(floatDir));
		float nDotH = Mathf::Clamp01(input.normal.Dot(floatDir));

		float D = GGXTerm(nDotH, input.roughness);
		float VF = 1.f / (4.f * lDotH * lDotH * (input.roughness + 0.5f));
		return input.albedo * nDotL + input.specColor * (D * VF * nDotL * input.specular);
	}

	// IBL //

	static Vector2 Hammersley2d(uint32_t i, uint32_t maxSampleCount)
	{
		static auto radicalInverseVdC = [](uint32_t bits)
		{
			bits = (bits << 16) | (bits >> 16);
			bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
			bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
			bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
			bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
			return float(bits) * 2.3283064365386963e-10f; // 0x100000000
		};

		return Vector2(float(i) / float(maxSampleCount), radicalInverseVdC(i));
	}

	static Vector3 ImportanceSampleGGX(Vector2 xi, float roughness, Vector3 normal)
	{
		float a = roughness * roughness;
		float phi = 2.f * Mathf::PI * xi.x;
		float cosTheta = Mathf::Sqrt((1 - xi.y) / (1.f + (a * a - 1) * xi.y));
		float sinTheta = Mathf::Sqrt(1 - cosTheta * cosTheta);
		Vector3 h;
		h.x = sinTheta * Mathf::Cos(phi);
		h.y = sinTheta * Mathf::Sin(phi);
		h.z = cosTheta;

		Vector3 upVector = Mathf::Abs(normal.z) < 0.999f ? Vector3::front : Vector3::right;
		Vector3 tangentX = upVector.Cross(normal).Normalize();
		Vector3 tangentY = normal.Cross(tangentX);
		return tangentX * h.x + tangentY * h.y + normal * h.z;
	}

	static Vector3 SpecularIBL(const CubemapPtr& cube, Vector3 specColor, float roughness, Vector3 n, Vector3 v)
	{
		Vector3 specLighting = Vector3::zero;
		const uint32_t numSamples = 128;
		for (uint32_t i = 0; i < numSamples; i++)
		{
			Vector2 xi = Hammersley2d(i, numSamples);
			Vector3 h = ImportanceSampleGGX(xi, roughness, n);
			Vector3 l = h * (2.f * v.Dot(h)) - v;
			float nDotV = Mathf::Clamp01(n.Dot(v));
			float nDotL = Mathf::Clamp01(n.Dot(l));
			float nDotH = Mathf::Clamp01(n.Dot(h));
			float vDotH = Mathf::Clamp01(v.Dot(h));
			if (nDotL > 0.f)
			{
				Vector3 sampleColor = IShader::TexCUBE(cube, l, 0.f).rgb;
				float V = SmithGGXVisibilityTerm(nDotV, nDotV, roughness);
				Vector3 F = FresnelTerm(vDotH, specColor);
				// pdf = D * nDotH / (4 * vDotH)
				// DVF * (nDotL) / pdf = (V * F * 4 * nDotL * vDotH) / nDotH
				specLighting += sampleColor * F * (V * 4.f * nDotL * vDotH / nDotH);
			}
		}
		return specLighting / float(numSamples);
	}

	static ColorRGB BRDF_IBL_GroundTruth(const PBSInput& input, const Vector3& lightDir, const Vector3& viewDir)
	{
		return SpecularIBL(input.envMap, input.specColor, input.roughness, input.normal, viewDir);
	}
};

} // namespace sr

#endif //! _SOFTRENDER_PBSF_H_
