#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/camera.h"
#include "base/mesh.h"
#include "math/mathf.h"

namespace rasterizer
{

void TestColor(Canvas* canvas);

struct Rasterizer
{
	static void DrawLine(Canvas* canvas, const Color32& color, int x0, int x1, int y0, int y1);
	static void DrawSmoothLine(Canvas* canvas, const Color32& color, float x0, float x1, float y0, float y1);
	static void DrawMeshPoint(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawTriangle(Canvas* canvas, const Color32& color, const Vector2& v0, const Vector2& v1, const Vector2& v2);
	static void DrawMeshColor(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

private:
	static float FloatPart(float v);
	static int IntPart(float v);
	static float Orient2D(const Vector2& v1, const Vector2& v2, const Vector2& p);
	static void Plot(Canvas* canvas, int x, int y, const Color32& color);
	static void Plot(Canvas* canvas, int x, int y, const Color32& color, bool swapXY);
	static void Plot(Canvas* canvas, int x, int y, const Color32& color, float alpha, bool swapXY = false);
};

}

#endif // !_RASTERIZER_H_
