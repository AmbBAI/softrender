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

struct IShader;
typedef std::shared_ptr<IShader> ShaderPtr;

struct IShader
{
	std::vector<VaryingDataElement> varyingDataDecl;
	int varyingDataSize;
	rawptr_t varyingData = nullptr;

	// alpha test
	bool isClipped;

	//uniform
	Matrix4x4 _MATRIX_MVP;
	Matrix4x4 _MATRIX_MV;
	Matrix4x4 _MATRIX_V;
	Matrix4x4 _MATRIX_P;
	Matrix4x4 _MATRIX_VP;
	Matrix4x4 _Object2World;
	Matrix4x4 _World2Object;

	// uniform camera
	Vector3 _WorldSpaceCameraPos;
	Vector4 _ScreenParams;
	Vector3 _ZBufferParams;

	// light
	Vector4 _WorldSpaceLightPos;
	Vector4 _LightColor;
	Vector4 _LightAtten; // atten0, atten1, atten2, range
	Vector3 _SpotLightDir;
	Vector3 _SpotLightParams; // cos(phi/2), cos(theta/2), falloff
	// uniform time
	//Vector4 _Time;
	//Vector4 _SinTime;
	//Vector4 _CosTime;
	//Vector4 _DeltaTime;

	virtual void _VSMain(const rawptr_t input) = 0;
	virtual Color _PSMain() = 0;
	virtual void _PassQuad(const rawptr_t quadVaryingData[4]) {}

	template<typename Type>
	static float CalcLod(const Type& ddx, const Type& ddy)
	{
		float delta = Mathf::Max(Type::Dot(ddx, ddx), Type::Dot(ddy, ddy));
		return Mathf::Max(0.f, 0.5f * Mathf::Log2(delta));
	}

	static Color Tex2D(const Texture2DPtr& tex, const Vector2& uv, float lod = 0.f)
	{
		if (tex == nullptr) return Color::white;
		return tex->Sample(uv, lod);
	}

	static Color Tex2D(const Texture2DPtr& tex, const Vector2& uv, const Vector2& ddx, const Vector2& ddy)
	{
		if (tex == nullptr) return Color::white;
		return tex->Sample(uv, ddx, ddy);
	}

	static float Tex2DProj(const Texture2DPtr& tex, const Vector2& uv, float depth, float bias)
	{
		if (tex == nullptr) return 0.f;
		return tex->SampleProj(uv, depth, bias);
	}

	static float Tex2DProjInterpolated(const Texture2DPtr& tex, const Vector2& uv, float depth, float bias)
	{
		if (tex == nullptr) return 0.f;
		float invW = 1.f / tex->GetWidth();
		float invH = 1.f / tex->GetHeight();
		float ret = 0.f;
		for (int xoff = -1; xoff <= 1; ++xoff)
		{
			for (int yoff = -1; yoff <= 1; ++yoff)
			{
				ret += tex->SampleProj(uv + Vector2(xoff * invW, yoff * invH), depth, bias);
			}
		}
		return ret / 9.f;
	}

	static Color TexCUBE(CubemapPtr& tex, const Vector3& s)
	{
		if (tex == nullptr) return Color::white;
		return tex->Sample(s);
	}

	static Matrix4x4 TangentSpaceRotation(const Vector3& tangent, const Vector3& binormal, const Vector3& normal)
	{
		return Matrix4x4::TBN(tangent, binormal, normal);
	}

	static Vector3 UnpackNormal(const Color& color, const Matrix4x4& tbn)
	{
		Vector3 normal = Vector3(color.r * 2.f - 1.f, color.g * 2.f - 1.f, color.b * 2.f - 1.f);
		return tbn.MultiplyVector(normal).Normalize();
	}
	
	static Vector3 Reflect(const Vector3& inDir, const Vector3& normal)
	{
		return inDir - normal * (normal.Dot(inDir) * 2.f);
	}

	bool Clip(float x)
	{
		return isClipped = (x < 0.f);
	}

	void InitLightArgs(const Vector3& worldPos, Vector3& lightDir, float& lightAtten)
	{
		if (Mathf::Approximately(_WorldSpaceLightPos.w, 0.f))
		{
			lightDir = -_WorldSpaceLightPos.xyz;
			lightAtten = 1.f;
		}
		else
		{
			lightDir = _WorldSpaceLightPos.xyz - worldPos;
			float distance = lightDir.Length();
			lightDir /= distance;
			if (_LightAtten.w < distance)
				lightAtten = 0.f;
			else
			{
				lightAtten = 1.f / (_LightAtten.x + _LightAtten.y * distance + _LightAtten.z * distance * distance);
				if (_SpotLightParams.x >= 0.f)
				{
					float spotLightFactor = ((-_SpotLightDir).Dot(lightDir) - _SpotLightParams.x) / (_SpotLightParams.y - _SpotLightParams.x);
					spotLightFactor = Mathf::Clamp01(Mathf::Pow(spotLightFactor, _SpotLightParams.z));
					lightAtten *= spotLightFactor;
				}
			}
		}
	}
};

template <typename VSInputType, typename VaryingDataType>
struct Shader : IShader
{
	Shader()
	{
		varyingDataDecl = VaryingDataType::GetDecl();
		varyingDataSize = sizeof(VaryingDataType);
	}

	void _VSMain(const rawptr_t input) override
	{
		*((VaryingDataType*)varyingData) = vert(*(VSInputType*)input);
	}

	void _PassQuad(const rawptr_t quadVaryingData[4]) override
	{
		passQuad(Quad<VaryingDataType*> {
				(VaryingDataType*)quadVaryingData[0],
				(VaryingDataType*)quadVaryingData[1],
				(VaryingDataType*)quadVaryingData[2],
				(VaryingDataType*)quadVaryingData[3]
		});
	}

	Color _PSMain() override
	{
		return frag(*(VaryingDataType*)(varyingData));
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
};

} // namespace rasterizer

#endif //! _RASTERIZER_SHADER_H_