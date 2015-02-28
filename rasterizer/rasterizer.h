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
	static void Line(Canvas* canvas, const Color& color, int x0, int x1, int y0, int y1);
	static void SmoothLine(Canvas* canvas, const Color& color, float x0, float x1, float y0, float y1);
	static void DrawMeshPoint(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform);
	static void DrawMeshWireFrame(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform);

private:
	static float FloatPart(float v);
	static int IntPart(float v);
	static void Plot(Canvas* canvas, int x, int y, const Color& color);
	static void Plot(Canvas* canvas, int x, int y, const Color& color, bool swapXY);
	static void Plot(Canvas* canvas, int x, int y, const Color& color, float alpha, bool swapXY = false);
};

}

#endif // !_RASTERIZER_H_
