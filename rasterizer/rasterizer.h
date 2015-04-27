#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/input.h"
#include "base/camera.h"
#include "math/mathf.h"
#include "math/transform.h"

#include "rasterizer/mesh.h"
#include "rasterizer/material.h"
#include "rasterizer/texture.h"
#include "rasterizer/clipper.hpp"
#include "rasterizer/vertex.hpp"
#include "rasterizer/light.hpp"
#include "rasterizer/shader.hpp"

namespace rasterizer
{

void TestColor(Canvas* canvas);
void TestTexture(Canvas* canvas, const Vector4& rect, const Texture& texture, float lod = 0.f);


template<typename Type>
static Type TriangleInterpolation(const Type& v0, const Type& v1, const Type& v2,
                                  float x, float y, float z)
{
	return v0 * x + v1 * y + v2 * z;
}

    
struct PSInput
{
    Vector3 position;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;

    PSInput() = default;
	PSInput(
		const VertexStd& v0,
		const VertexStd& v1,
		const VertexStd& v2,
		float x, float y, float z)
	{
        position = TriangleInterpolation(v0.position, v1.position, v2.position, x, y, z);
		uv = TriangleInterpolation(v0.texcoord, v1.texcoord, v2.texcoord, x, y, z);
		normal = TriangleInterpolation(v0.normal, v1.normal, v2.normal, x, y, z);
		tangent = TriangleInterpolation(v0.tangent, v1.tangent, v2.tangent, x, y, z);
		bitangent = TriangleInterpolation(v0.bitangent, v1.bitangent, v2.bitangent, x, y, z);
	}
};

struct Shader0 : Shader < VertexStd, PSInput >
{
	const Vector3* position = nullptr;
	const Vector3* normal = nullptr;
	const Vector4* tangent = nullptr;
	const Vector2* texcoord = nullptr;
    
    PSInput quad[4];

	void VertexShader(VertexStd& out) override
	{
		out.hc = _MATRIX_MVP.MultiplyPoint(*position);
        out.position = _Object2World.MultiplyPoint3x4(*position);
		out.normal = _Object2World.MultiplyVector(*normal).Normalize();
        if (tangent != nullptr)
        {
            out.tangent = _Object2World.MultiplyVector(tangent->xyz).Normalize();
            out.bitangent = normal->Cross(tangent->xyz) * tangent->w;
            out.bitangent = _Object2World.MultiplyVector(out.bitangent).Normalize();
        }
        if (texcoord != nullptr)
        {
            out.texcoord = *texcoord;
        }
		out.clipCode = Clipper::CalculateClipCode(out.hc);
	}

    const Vector2 ddx() const
    {
        return quad[1].uv - quad[0].uv;
    }
    
    const Vector2 ddy() const
    {
        return quad[2].uv - quad[0].uv;
    }
    
    const float calc_lod(u32 width, u32 height) const
    {
        Vector2 dx = ddx() * (float)width;
        Vector2 dy = ddy() * (float)height;
        float d = Mathf::Max(dx.Dot(dx), dy.Dot(dy));
        return 0.5f * Mathf::Log2(d);
    }
    
	const Color PixelShader(const PSInput& input) override
	{
        LightInput lightInput;
        lightInput.ambient = material->ambient;
		lightInput.diffuse = material->diffuse;
        lightInput.specular = material->specular;
		lightInput.shininess = material->shininess;
		//TODO Alpha

		if (material && material->diffuseTexture)
		{
			u32 width = material->diffuseTexture->GetWidth();
			u32 height = material->diffuseTexture->GetHeight();
			float lod = calc_lod(width, height);

			//float lodDepth = 1.f - lod / 10.f;
			//Color texColor = Color(lodDepth, lodDepth, lodDepth, lodDepth);
			Color texColor = material->diffuseTexture->Sample(input.uv.x, input.uv.y, lod);
			lightInput.ambient = lightInput.ambient.Modulate(texColor);
            lightInput.diffuse = lightInput.diffuse.Modulate(texColor);
		}

		Vector3 normal = input.normal;
		if (material && material->normalTexture)
		{
			Vector3 tangent = input.tangent;
			Vector3 bitangent = input.bitangent;
			Matrix4x4 tbn = Matrix4x4::TBN(tangent, bitangent, normal);

			u32 width = material->normalTexture->GetWidth();
			u32 height = material->normalTexture->GetHeight();
			float lod = calc_lod(width, height);

			Color normalColor = material->normalTexture->Sample(input.uv.x, input.uv.y, lod);
			normal = Vector3(normalColor.r * 2 - 1, normalColor.g * 2 - 1, normalColor.b * 2 - 1);

			normal = tbn.MultiplyVector(normal).Normalize();
		}
        
        if (material && material->specularTexture)
        {
            lightInput.specular = material->specularTexture->Sample(input.uv.x, input.uv.y);
        }

        Color output;
		//output.r = (normal.x + 1.f) * 0.5f;
		//output.g = (normal.y + 1.f) * 0.5f;
		//output.b = (normal.z + 1.f) * 0.5f;

		if (light)
		{
			//output = LightingLambert(lightInput, normal, light->direction, light->intensity);
            
            Vector3 lightDir = light->direction.Negate();
            float attenuation = 1.f;
            float intensity = light->intensity;
            switch (light->type) {
                case Light::LightType_Directional:
                    break;
                case Light::LightType_Point:
                    {
                        lightDir = light->position - input.position;
                        float distance = lightDir.Length();
                        lightDir /= distance;
                        attenuation = light->range / (light->atten0 + light->atten1 * distance + light-> atten2 * distance * distance);
                    }
                    break;
                case Light::LightType_Spot:
                    {
                        lightDir = light->position - input.position;
                        float distance = lightDir.Length();
                        lightDir /= distance;
                        attenuation = light->range / (light->atten0 + light->atten1 * distance + light-> atten2 * distance * distance);

                        float scale = (light->direction.Dot(lightDir) - light->cosHalfPhi) / (light->cosHalfTheta - light->cosHalfPhi);
                        scale = Mathf::Clamp01(Mathf::Pow(scale, light->falloff));
                        intensity = intensity * scale;

                    }
                    break;
            }
            Color lightColor = light->color;
            lightColor.rgb = lightColor.rgb * intensity;
            Vector3 viewDir = (camera->GetPosition() - input.position).Normalize();
			output = LightingPhong(lightInput, normal, lightDir, lightColor, viewDir, attenuation);
        } else output = lightInput.ambient;

        return output;
	}
};

struct Rasterizer
{
	static Vector3 lightDir;
	static Canvas* canvas;
	static CameraPtr camera;
	static MaterialPtr material;
    static LightPtr light;

	static Shader0 shader;

	static bool isDrawPoint;
	static bool isDrawWireFrame;
	static bool isDrawTextured;

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawTriangle(const Projection& p0, const Projection& p1, const Projection& p2, const Triangle<VertexStd>& triangle);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform);

	static int Orient2D(int x0, int y0, int x1, int y1, int x2, int y2);
};

}

#endif // !_RASTERIZER_H_
