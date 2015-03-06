#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/camera.h"
#include "base/mesh.h"
#include "base/texture.h"
#include "math/mathf.h"

namespace rasterizer
{

void TestColor(Canvas* canvas);

struct Rasterizer
{
	struct Point2D
	{
		int x = 0;
		int y = 0;
		float depth = 0.0f;

		Point2D(int _x, int _y) : x(_x), y(_y) {}
		Point2D(int _x, int _y, float _depth) : x(_x), y(_y), depth(_depth) {}
	};

	struct Vertex
	{
		Vector3 position = Vector3::zero;
		Vector3 normal = Vector3::up;
		Vector2 texcoord = Vector2::zero;
		Vector3 projection = Vector3::zero;
		Point2D vpPoint = Point2D(0, 0);
	};

	struct Face
	{
		Vertex v[3];
	};

	static Vector3 lightDir;
	static Canvas* canvas;
	static Camera* camera;
	static Texture* texture;

	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color);
	static void DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	//static void DrawTriangle(const Vector2& v0, const Vector2& v1, const Vector2& v2, const Color32& color);
	static void DrawTriangle(const Point2D& v0, const Point2D& v1, const Point2D& v2, const Color32& color);
	static void DrawMeshColor(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color32& color);
	static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

private:
	static float FloatPart(float v);
	static int IntPart(float v);
	static float Orient2D(const Vector2& v1, const Vector2& v2, const Vector2& p);
	static float Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p);
	static void Plot(int x, int y, const Color32& color);
	static void Plot(int x, int y, const Color32& color, bool swapXY);
	static void Plot(int x, int y, const Color32& color, float alpha, bool swapXY = false);
};

}

#endif // !_RASTERIZER_H_
