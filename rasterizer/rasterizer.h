#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/input.h"
#include "base/camera.h"
#include "math/mathf.h"

#include "rasterizer/mesh.h"
#include "rasterizer/texture.h"
#include "rasterizer/clipper.hpp"
#include "rasterizer/vertex.hpp"
#include "rasterizer/material.hpp"
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
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;

    PSInput() = default;
	PSInput(
		const VertexStd& v0,
		const VertexStd& v1,
		const VertexStd& v2,
		float x, float y, float z)
	{
		uv = TriangleInterpolation(v0.texcoord, v1.texcoord, v2.texcoord, x, y, z);
		normal = TriangleInterpolation(v0.normal, v1.normal, v2.normal, x, y, z);
		tangent = TriangleInterpolation(v0.tangent, v1.tangent, v2.tangent, x, y, z);
	}
};

struct Shader0 : Shader < VertexStd, PSInput >
{
	const Vector3* position = nullptr;
	const Vector3* normal = nullptr;
	const Vector3* tangent = nullptr;
	const Vector2* texcoord = nullptr;
    
    PSInput quad[4];

	void VertexShader(VertexStd& out) override
	{
		out.position = _MATRIX_MVP.MultiplyPoint(*position);
		out.normal = _Object2World.MultiplyVector(*normal);
		out.tangent = _Object2World.MultiplyVector(*tangent);
		out.texcoord = *texcoord;
		out.clipCode = Clipper::CalculateClipCode(out.position);
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
		Color color = Color::white;

		if (material && material->diffuseTexture)
		{
            u32 width = material->diffuseTexture->GetWidth();
            u32 height = material->diffuseTexture->GetHeight();
            float lod = calc_lod(width, height);
			//float lodDepth = 1.f - lod / 10.f;
			//Color texColor = Color(lodDepth, lodDepth, lodDepth, lodDepth);
			Color texColor = material->diffuseTexture->Sample(input.uv.x, input.uv.y, lod);
			color = color.Modulate(texColor);
		}

		Vector3 normal = input.normal;
		if (material && material->normalTexture)
		{
			Vector3 binormal = normal.Cross(input.tangent).Normalize();
			Matrix4x4 tbn = Matrix4x4::TBN(input.tangent, binormal, normal);

			Color normalColor = material->normalTexture->Sample(input.uv.x, input.uv.y);
			normal = Vector3(normalColor.r * 2 - 1, normalColor.g * 2 - 1, normalColor.b * 2 - 1);

			normal = tbn.MultiplyVector(normal);
		}

        if (light)
        {
            color = LightingLambert(color, normal, light->direction);
        }
		return color;
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

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color);
	static void DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

	static void DrawTriangle(const Projection& p0, const Projection& p1, const Projection& p2, const Triangle<VertexStd>& triangle);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform);

	static int Orient2D(int x0, int y0, int x1, int y1, int x2, int y2);
	static void Plot(int x, int y, const Color32& color, float alpha, bool swapXY = false);
};

}

#endif // !_RASTERIZER_H_
