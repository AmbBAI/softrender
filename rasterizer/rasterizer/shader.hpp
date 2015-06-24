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

struct ShaderBase
{
	RenderData* renderData;

	std::vector<VaryingDataDecl::Layout> varyingDataDecl;
	int varyingDataSize;

	VertexOutData* vertexOut = nullptr;
	PixelInData* pixelIn = nullptr;

	Matrix4x4 _MATRIX_MVP;
	Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
	Matrix4x4 _MATRIX_P;
	Matrix4x4 _MATRIX_VP;
	Matrix4x4 _Object2World;
	Matrix4x4 _World2Object;

	virtual void vsMain(const void* input) = 0;
	virtual Color psMain() = 0;
};

template <typename VSInputType, typename VaryingDataType>
struct Shader : ShaderBase
{
	void vsMain(const void* input) override
	{
		VSInputType* vertexInput = (VSInputType*)input;
		*((VaryingDataType*)vertexOut->data) = vert(*vertexInput);
		auto decl = vertexOut->varyingDataBuffer->GetVaryingDataDecl();
		vertexOut->position = *Buffer::Value<Vector4>(vertexOut->data, decl.positionOffset);
		vertexOut->clipCode = Clipper::CalculateClipCode(vertexOut->position);
	}

	Color psMain() override
	{
		return frag(*(VaryingDataType*)(pixelIn->data));
	}

	virtual VaryingDataType vert(const VSInputType& input)
	{
		VaryingDataType output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		return output;
	}

	virtual Color frag(const VaryingDataType& input)
	{
		return Color::white;
	}

	Vector3 cameraPosition;
	Vector3 GetViewDir(const Vector3 position)
	{
		return (cameraPosition - position).Normalize();
	}

	typedef Color(*LightFunc)(const LightInput& input, const Vector3& normal, const Vector3& lightDir, const Color& lightColor, const Vector3& viewDir, float attenuation);
	Color CalcLight(const LightInput& input, const LightPtr& light, LightFunc lightFunc, const Vector3& position, const Vector3& normal)
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
};

} // namespace rasterizer

#endif //! _RASTERIZER_SHADER_H_