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

/*
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

struct PS : shader::Shader<Vertex, VertexOutData>
{
	Vector2 ddx, ddy;

	const float calcLOD(u32 width, u32 height) const
	{
		Vector2 dx = ddx * (float)width;
		Vector2 dy = ddy * (float)height;
		float d = Mathf::Max(dx.Dot(dx), dy.Dot(dy));
		return 0.5f * Mathf::Log2(d);
	}

	const float clacTexLod(TexturePtr tex) const
	{
		u32 width = tex->GetWidth();
		u32 height = tex->GetHeight();
		return calcLOD(width, height);
	}

	const Color psMain() override
	{
		shader::LightInput lightInput;
		lightInput.ambient = material->ambient;
		lightInput.diffuse = material->diffuse;
		lightInput.specular = material->specular;
		lightInput.shininess = material->shininess;

		if (material && material->diffuseTexture)
		{

			float lod = clacTexLod(material->diffuseTexture);
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

			float lod = clacTexLod(material->normalTexture);
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

		shader::CalcLight(lightInput, Rasterizer::light, shader::LightingBlinnPhong, input.position, normal);

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
*/

struct RenderState
{
	enum ZTestType
	{
		ZTestType_Always = 0,
		ZTestType_Less,
		ZTestType_Greater,
		ZTestType_LEqual,
		ZTestType_GEqual,
		ZTestType_Equal,
		ZTestType_NotEqual
	} ztest = ZTestType_LEqual;
	bool zWrite = true;

	enum CullType
	{
		CullType_Off = 0,
		CullType_Back,
		CullType_Front,
	} cull = CullType_Back;
};

struct Rasterizer
{
	static RenderState renderState;
	static RenderData renderData;
	static VaryingDataBuffer varyingDataBuffer;

	static Matrix4x4 transform;
	static Canvas* canvas;
	static CameraPtr camera;
    static LightPtr light;
	static MaterialPtr material;
	static ShaderBase* shader;

	static bool isDrawPoint;
	static bool isDrawWireFrame;
	static bool isDrawTextured;
	static int pixelDrawCount;
	static int triangleDrawCount;

    static void Initialize();
    
	static void SetCamera(CameraPtr camera);
	static void SetMainLight(LightPtr light);
	static void SetShader(ShaderBase* shader);
	static void SetTransform(const Matrix4x4& transform);
	static void Submit();

	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawTriangle(Triangle<std::pair<Projection, VertexVaryingData> > triangle);
	static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform);

	static void PrepareRender();
	static void Render();
};

}

#endif // !_RASTERIZER_H_
