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
#include "rasterizer/render_data.h"
#include "rasterizer/clipper.hpp"
#include "rasterizer/vertex.hpp"
#include "rasterizer/light.hpp"
#include "rasterizer/shader.hpp"

namespace rasterizer
{

void TestColor(Canvas* canvas);
void TestTexture(Canvas* canvas, const Vector4& rect, const Texture& texture, float lod = 0.f);


template<typename Type>
static Type TriInterp(const Type& v0, const Type& v1, const Type& v2,
                                  float x, float y, float z)
{
	return v0 * x + v1 * y + v2 * z;
}

    
struct Pixel
{
    Vector3 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;

    Pixel() = default;
	Pixel(
		const Vertex& v0,
		const Vertex& v1,
		const Vertex& v2,
		float x, float y, float z)
	{
        position = TriInterp(v0.position, v1.position, v2.position, x, y, z);
		texcoord = TriInterp(v0.texcoord, v1.texcoord, v2.texcoord, x, y, z);
		normal = TriInterp(v0.normal, v1.normal, v2.normal, x, y, z);
		tangent = TriInterp(v0.tangent, v1.tangent, v2.tangent, x, y, z);
		bitangent = TriInterp(v0.bitangent, v1.bitangent, v2.bitangent, x, y, z);
	}
};

struct VS : VertexShader < Vertex >
{
	const RenderMeshData<Vertex>* mesh;
	const RenderVertexData<Vertex>* vertex;
	VS(const RenderMeshData<Vertex>& _mesh, const RenderVertexData<Vertex>& _vertex)
		: mesh(&_mesh), vertex(&_vertex) { }

	void vsMain(Vertex& out) override
	{
		out.hc = mesh->_MATRIX_MVP.MultiplyPoint(vertex->position);
		out.position = mesh->_Object2World.MultiplyPoint3x4(vertex->position);
		out.normal = mesh->_Object2World.MultiplyVector(vertex->normal).Normalize();

		out.tangent = mesh->_Object2World.MultiplyVector(vertex->tangent.xyz).Normalize();
		out.bitangent = vertex->normal.Cross(vertex->tangent.xyz) * vertex->tangent.w;
		out.bitangent = mesh->_Object2World.MultiplyVector(out.bitangent).Normalize();
		out.texcoord = vertex->texcoord;

		out.clipCode = Clipper::CalculateClipCode(out.hc);
	}
};

struct PS : PixelShader < Pixel >
{
	MaterialPtr material = nullptr;
	LightPtr light = nullptr;
	CameraPtr camera = nullptr;
	Vector2 ddx, ddy;

	const float calcLOD(u32 width, u32 height) const
	{
		Vector2 dx = ddx * (float)width;
		Vector2 dy = ddy * (float)height;
		float d = Mathf::Max(dx.Dot(dx), dy.Dot(dy));
		return 0.5f * Mathf::Log2(d);
	}

	const Color psMain(const Pixel& input) override
	{
		LightInput lightInput;
		lightInput.ambient = material->ambient;
		lightInput.diffuse = material->diffuse;
		lightInput.specular = material->specular;
		lightInput.shininess = material->shininess;

		if (material && material->diffuseTexture)
		{
			u32 width = material->diffuseTexture->GetWidth();
			u32 height = material->diffuseTexture->GetHeight();
			float lod = calcLOD(width, height);

			//float lodDepth = 1.f - lod / 10.f;
			//Color texColor = Color(lodDepth, lodDepth, lodDepth, lodDepth);
			Color texColor = material->diffuseTexture->Sample(input.texcoord.x, input.texcoord.y, lod);
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
			float lod = calcLOD(width, height);

			Color normalColor = material->normalTexture->Sample(input.texcoord.x, input.texcoord.y, lod);
			normal = Vector3(normalColor.r * 2 - 1, normalColor.g * 2 - 1, normalColor.b * 2 - 1);

			normal = tbn.MultiplyVector(normal).Normalize();
		}

		if (material && material->specularTexture)
		{
			lightInput.specular = material->specularTexture->Sample(input.texcoord.x, input.texcoord.y);
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
				attenuation = light->range * light->CalcAtten(distance);
			}
			break;
			case Light::LightType_Spot:
			{
				lightDir = light->position - input.position;
				float distance = lightDir.Length();
				lightDir /= distance;
				attenuation = light->range * light->CalcAtten(distance);

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
		}
		else output = lightInput.ambient;

		if (material && material->isTransparent)
		{
			output.a = material->alpha;

			if (material->alphaMaskTexture)
			{
				Color alpha = material->alphaMaskTexture->Sample(input.texcoord.x, input.texcoord.y);
				output.a = output.a * (1.f - alpha.b);
			}
		}
		else output.a = 1.f;

		return output;
	}
};

struct Rasterizer
{
	static RenderData<Vertex, Pixel, 2> renderData;

	static Canvas* canvas;
	static CameraPtr camera;
    static LightPtr light;

	static bool isDrawPoint;
	static bool isDrawWireFrame;
	static bool isDrawTextured;

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawTriangle(u32 meshIndex, u32 triangleIndex);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform);

	static int Orient2D(int x0, int y0, int x1, int y1, int x2, int y2);

	static void PrepareRender();
	static void Render();
};

}

#endif // !_RASTERIZER_H_
