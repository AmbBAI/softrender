#ifndef _SOFTRENDER_PBSF_HPP_
#define _SOFTRENDER_PBSF_HPP_

#include "base/header.h"
#include "math/color.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/mathf.h"
#include "shader.hpp"

namespace sr
{

struct PBSLight
{
	Vector3 dir;
	Vector3 color;
};

struct PBSInput
{
	Vector3 albedo;
	Vector3 normal;
	float roughness;
	float metallic;

	Vector3 diffColor;
	Vector3 specColor;
	float reflectivity;

	void PBSSetup()
	{
		static float dielectricSpec = 0.220916301f;
		static Vector3 dieletctricSpecRGB = Vector3::one * dielectricSpec;
		specColor = Mathf::LinearInterp(dieletctricSpecRGB, albedo, metallic);
		reflectivity = Mathf::Lerp(metallic, 1.f, dielectricSpec);
		diffColor = albedo * (1.f - reflectivity);
	}
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

	static float BlinnPhongTerm(float nDotH, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float specPower = Mathf::Max(2.f / (a2 + 1e-4f) - 2.f, 1e-4f);
		return 4.f * Mathf::Pow(nDotH, specPower) / (a2 + 1e-4f);
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

	static Vector3 FresnelTerm(float vDotH, const Vector3& specular)
	{
		return specular + (Vector3::one - specular) * Mathf::Pow(1.f - vDotH, 5.f);
	}

	static float DisneyDiffuseTerm(float nDotL, float nDotV, float lDotH, float roughness)
	{
		float nlPow5 = Mathf::Pow(1.f - nDotL, 5.f);
		float nvPow5 = Mathf::Pow(1.f - nDotV, 5.f);
		float fD90 = 0.5f + 2.f * lDotH * lDotH * roughness;
		return (1.f + (fD90 - 1.f) * nlPow5) * (1.f + (fD90 - 1.f) * nvPow5);
	}

	static Vector3 BRDF1(const PBSInput& input, const Vector3& normal, const Vector3& viewDir, const PBSLight& light)
	{
		Vector3 halfDir = (light.dir + viewDir).Normalize();
		float nDotL = Mathf::Clamp01(normal.Dot(light.dir));
		float nDotV = Mathf::Clamp01(normal.Dot(viewDir));
		float nDotH = Mathf::Clamp01(normal.Dot(halfDir));
		float vDotH = Mathf::Clamp01(viewDir.Dot(halfDir));
		float lDotH = Mathf::Clamp01(light.dir.Dot(halfDir));

		float diffuseTerm = DisneyDiffuseTerm(nDotL, nDotV, lDotH, input.roughness);
		float D = GGXTerm(nDotH, input.roughness);
		float V = SmithGGXVisibilityTerm(nDotL, nDotV, input.roughness);
		Vector3 F = FresnelTerm(vDotH, input.specColor);
		return input.diffColor * (diffuseTerm * nDotL) + light.color * F * (D * V * Mathf::PI * nDotL);
	}

	static Vector3 BRDF2(const PBSInput& input, const Vector3& normal, const Vector3& viewDir, const PBSLight& light)
	{
		Vector3 halfDir = (light.dir + viewDir).Normalize();
		float nDotL = Mathf::Clamp01(normal.Dot(light.dir));
		float lDotH = Mathf::Clamp01(light.dir.Dot(halfDir));
		float nDotH = Mathf::Clamp01(normal.Dot(halfDir));

		float D = GGXTerm(nDotH, input.roughness);
		float VF = 1.f / (4.f * lDotH * lDotH * (input.roughness + 0.5f));
		return input.diffColor * nDotL + light.color * (D * VF * nDotL * Mathf::PI);
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
		float cosTheta = Mathf::Sqrt((1.f - xi.y) / (1.f + (a * a - 1.f) * xi.y));
		float sinTheta = Mathf::Sqrt(1.f - cosTheta * cosTheta);
		Vector3 h;
		h.x = sinTheta * Mathf::Cos(phi);
		h.y = sinTheta * Mathf::Sin(phi);
		h.z = cosTheta;

		Vector3 upVector = Mathf::Abs(normal.z) < 0.999f ? Vector3::front : Vector3::right;
		Vector3 tangentX = upVector.Cross(normal).Normalize();
		Vector3 tangentY = normal.Cross(tangentX);
		return tangentX * h.x + tangentY * h.y + normal * h.z;
	}

	static Vector3 GroundTruthSpecularIBL(const Cubemap& cube, Vector3 specColor, Vector3 n, Vector3 v, float roughness, uint32_t sampleCount)
	{
		Vector3 specLighting = Vector3::zero;
		for (uint32_t i = 0; i < sampleCount; i++)
		{
			Vector2 xi = Hammersley2d(i, sampleCount);
			Vector3 h = ImportanceSampleGGX(xi, roughness, n);
			float vDotH = Mathf::Clamp01(v.Dot(h));
			Vector3 l = h * (2.f * vDotH) - v;
			float nDotV = Mathf::Clamp01(n.Dot(v));
			float nDotL = Mathf::Clamp01(n.Dot(l));
			float nDotH = Mathf::Clamp01(n.Dot(h));
			if (nDotL > 0.f)
			{
				Vector3 sampleColor = IShader::TexCUBE(cube, l, 0.f).rgb;
				float V = SmithGGXVisibilityTerm(nDotL, nDotV, roughness);
				Vector3 F = FresnelTerm(vDotH, specColor);
				// pdf = D * nDotH / (4 * vDotH)
				// DVF * (nDotL) / pdf = (V * F * 4 * nDotL * vDotH) / nDotH
				specLighting += sampleColor * F * (V * 4.f * nDotL * vDotH / (nDotH + Mathf::epsilon));
			}
		}
		return specLighting / float(sampleCount);
	}

	static Vector3 PrefilterEnvMap(const Cubemap& cube, float roughness, Vector3 r, uint32_t sampleCount)
	{
		Vector3 n = r;
		Vector3 v = r;

		Vector3 prefilterColor = Vector3::zero;
		float totalWeight = Mathf::epsilon;
		for (uint32_t i = 0; i < sampleCount; i++)
		{
			Vector2 xi = Hammersley2d(i, sampleCount);
			Vector3 h = ImportanceSampleGGX(xi, roughness, n);
			Vector3 l = h * (2.f * v.Dot(h)) - v;

			float nDotL = n.Dot(l);
			if (nDotL > 0.f)
			{
				prefilterColor += IShader::TexCUBE(cube, l, 0.f).rgb * nDotL;
				totalWeight += nDotL;
			}
		}
		return prefilterColor / totalWeight;
	}

	static Vector2 IntergrateBRDF(float roughness, float nDotV, uint32_t sampleCount)
	{
		Vector3 v; // n = (0, 0, 1);
		v.x = Mathf::Sqrt(1.f - nDotV * nDotV);
		v.y = 0.f;
		v.z = nDotV;

		float a = 0.f;
		float b = 0.f;
		for (uint32_t i = 0; i < sampleCount; ++i)
		{
			Vector2 xi = Hammersley2d(i, sampleCount);
			Vector3 h = ImportanceSampleGGX(xi, roughness, Vector3::front);
			Vector3 l = h * (2.f * v.Dot(h)) - v;

			float nDotL = Mathf::Clamp01(l.z);
			float nDotH = Mathf::Clamp01(h.z);
			float vDotH = Mathf::Clamp01(v.Dot(h));
			if (nDotL > 0.f)
			{
				float V = SmithGGXVisibilityTerm(nDotL, nDotV, roughness);
				// (V * 4 * nDotL * vDotH) / nDotH
				float G = V * 4.f * nDotL * vDotH / nDotH;
				float Fc = Mathf::Pow(1.f - vDotH, 5.f);
				a += (1.f - Fc) * G;
				b += Fc * G;
			}
		}

		a /= (float)sampleCount;
		b /= (float)sampleCount;
		return Vector2(a, b);
	}

	static Vector3 ApproximateSpecularIBL(const Cubemap& cubemap, const Vector3& specColor, const Vector3& normal, const Vector3& viewDir, float roughness)
	{
		float nDotV = Mathf::Clamp01(normal.Dot(viewDir));
		Vector3 r = normal * (2.f * nDotV) - viewDir;
		static Texture2DPtr iblLUT = Texture2D::LoadTexture("resources/pbr/lut.png");

		Vector3 prefilterColor = cubemap.Sample(r, roughness).rgb;

		Vector3 envBRDF = iblLUT->Sample(Vector2(roughness, nDotV), 0.f).rgb;
		return prefilterColor * (specColor * envBRDF.x + Vector3::one * envBRDF.y);
	}

};

} // namespace sr

#endif //! _SOFTRENDER_PBSF_HPP_
