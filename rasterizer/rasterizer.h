#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_

#include "base/header.h"
#include "base/application.h"
#include "base/canvas.h"
#include "base/input.h"
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
		float w = 0.0f;

        Point2D() = default;
		Point2D(int _x, int _y) : x(_x), y(_y) {}
		Point2D(int _x, int _y, float _depth) : x(_x), y(_y), w(_depth) {}
	};

	struct Vertex
	{
		Vector4 position = Vector3::zero;
		Vector3 normal = Vector3::up;
		Vector3 tangent = Vector3::right;
		Vector2 texcoord = Vector2::zero;
		Point2D point = Point2D(0, 0);
        u32 clipCode;
	};

	struct Face
	{
		Vertex v[3];
	};
    
    struct Plane
    {
        u32 cullMask = 0x0;
        typedef float (*ClippingFunc)(const Vector4& v0, const Vector4& v1);
        ClippingFunc clippingFunc = nullptr;
    };
    static Plane viewFrustumPlanes[6];

	static Vector3 lightDir;
	static Canvas* canvas;
	static CameraPtr camera;
	static MaterialPtr material;
	typedef Color (*FragmentShader)(const Face& face, float w0, float w1, float w2, float invW);
	static FragmentShader fragmentShader;

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color);
	static void DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

    static Color FS(const Face& face, float w0, float w1, float w2, float invW);
    static void DrawTriangle(const Face& f);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color& color);

    static Vertex ClipLineFromPlane(float t, const Vertex& v0, const Vertex& v1);
    static std::vector<Face> ClipTriangleFromPlane(const Face& face, const Plane& plane);
    static std::vector<Face> ClipTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    
private:
	static float FloatPart(float v);
	static int IntPart(float v);
	static int Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p);
	static void Plot(int x, int y, const Color32& color);
	static void Plot(int x, int y, const Color32& color, float alpha, bool swapXY = false);

	static float ClipLine(float f0, float w0, float f1, float w1);
	static u32 CalculateClipCode(const Vector4& position);
	static Point2D CalculateViewPoint(const Vector4& position);

    static std::vector<Vertex> vertices;
};

}

#endif // !_RASTERIZER_H_
