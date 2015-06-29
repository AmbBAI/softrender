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

	std::vector<VaryingDataLayout> varyingDataDecl;
	int varyingDataSize;

	VertexVaryingData* vertexVaryingData = nullptr;
	PixelVaryingData* pixelVaryingData = nullptr;

	Matrix4x4 _MATRIX_MVP;
	Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
	Matrix4x4 _MATRIX_P;
	Matrix4x4 _MATRIX_VP;
	Matrix4x4 _Object2World;
	Matrix4x4 _World2Object;
	Vector3 _WorldSpaceCameraPos;
	Vector3 _lightPosition;

	virtual void _VSMain(const void* input) = 0;
	virtual Color _PSMain() = 0;
	virtual void _PassQuad(const Quad<PixelVaryingData>& quadVaryingData) {}

	template<typename Type>
	static float CalcLod(const Type& ddx, const Type& ddy)
	{
		float delta = Mathf::Max(Type::Dot(ddx, ddx), Type::Dot(ddy, ddy));
		return Mathf::Max(0.f, 0.5f * Mathf::Log2(delta));
	}

	static Color Tex2D(TexturePtr& tex, const Vector2& uv)
	{
		return Tex2D(tex, uv, 0.f);
	}

	static Color Tex2D(TexturePtr& tex, const Vector2& uv, float lod)
	{
		if (tex == nullptr) return Color::white;
		return tex->Sample(uv.x, uv.y, lod);
	}

	static Color Tex2D(TexturePtr& tex, const Vector2& uv, const Vector2& ddx, const Vector2& ddy)
	{
		if (tex == nullptr) return Color::white;
		float width = (float)tex->GetWidth();
		float height = (float)tex->GetHeight();
		float lod = CalcLod(ddx * width, ddy * height);
		return Tex2D(tex, uv, lod);
	}

	static const Matrix4x4 GetTangentSpace(const Vector3& tangent, const Vector3& binormal, const Vector3& normal)
	{
		return Matrix4x4::TBN(tangent, binormal, normal);
	}

	static const Vector3 UnpackNormal(const Color& color, const Matrix4x4& tbn)
	{
		Vector3 normal = Vector3(color.r * 2.f - 1.f, color.g * 2.f - 1.f, color.b * 2.f - 1.f);
		return tbn.MultiplyVector(normal).Normalize();
	}
};

template <typename VSInputType, typename VaryingDataType>
struct Shader : ShaderBase
{
	void _VSMain(const void* input) override
	{
		VSInputType* vertexInput = (VSInputType*)input;
		*((VaryingDataType*)vertexVaryingData->data) = vert(*vertexInput);
		auto decl = vertexVaryingData->varyingDataBuffer->GetVaryingDataDecl();
		vertexVaryingData->position = *Buffer::Value<Vector4>(vertexVaryingData->data, decl.positionOffset);
		vertexVaryingData->clipCode = Clipper::CalculateClipCode(vertexVaryingData->position);
	}

	void _PassQuad(const Quad<PixelVaryingData>& quadVaryingData) override
	{
		passQuad(Quad<VaryingDataType*> {
				(VaryingDataType*)quadVaryingData[0].data,
				(VaryingDataType*)quadVaryingData[1].data,
				(VaryingDataType*)quadVaryingData[2].data,
				(VaryingDataType*)quadVaryingData[3].data
		});
	}

	Color _PSMain() override
	{
		return frag(*(VaryingDataType*)(pixelVaryingData->data));
	}

	virtual VaryingDataType vert(const VSInputType& input)
	{
		VaryingDataType output;
		output.position = _MATRIX_MVP.MultiplyPoint(input.position);
		return output;
	}

	virtual void passQuad(const Quad<VaryingDataType*>& quad)
	{
	}

	virtual Color frag(const VaryingDataType& input)
	{
		return Color::white;
	}

	Vector3 GetViewDir(const Vector3 position)
	{
		return (_WorldSpaceCameraPos - position).Normalize();
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