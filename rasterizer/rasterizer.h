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
		float invZ = 0.0f;
		float invW = 0.0f;

        Point2D() = default;
		Point2D(int _x, int _y) : x(_x), y(_y) {}
		Point2D(int _x, int _y, float _depth) : x(_x), y(_y), invW(_depth) {}
	};

    struct Vertex_p
    {
        Vector4 position = Vector4();
        u32 clipCode = 0x0;
        
        //static Vertex_p Lerp(const Vertex_p& a, const Vertex_p& b, float t);
    };
    
	struct Vertex
	{
		Vector4 position = Vector4();
		Vector3 normal = Vector3::up;
		Vector3 tangent = Vector3::right;
		Vector2 texcoord = Vector2::zero;
		Point2D point = Point2D(0, 0);
        u32 clipCode = 0x0;
        
        //static Vertex Lerp(const Vertex& a, const Vertex& b, float t);
	};

    template<class VertexType, int VertexCount>
	struct Face
	{
		VertexType v[VertexCount];
	};
    
    typedef Face<Vertex, 3> Triangle;
    typedef Face<Vertex_p, 2> Line;
    
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
	typedef Color (*FragmentShader)(const Triangle& face, float w0, float w1, float w2, float invW);
	static FragmentShader fragmentShader;

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color);
	static void DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

    static Color FS(const Triangle& face, float w0, float w1, float w2, float invW);
    static void DrawTriangle(const Triangle& f);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color& color);
    
    static std::vector<Line> ClipLine(const Vertex_p& v0, const Vertex_p& v1);
    static std::vector<Triangle> ClipTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    
private:
	static int Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p);
	static void Plot(int x, int y, const Color32& color);
	static void Plot(int x, int y, const Color32& color, float alpha, bool swapXY = false);

	static float Clip(float f0, float w0, float f1, float w1);
    static Vertex LerpVertex(const Vertex& v0, const Vertex& v1, float t);
    static Vertex_p LerpVertex(const Vertex_p& v0, const Vertex_p& v1, float t);
    static void ClipLineFromPlane(std::vector<Line>& clippedLines, const Line& line, const Plane& plane);
    static void ClipTriangleFromPlane(std::vector<Triangle>& clippedTriangles, const Triangle& face, const Plane& plane);
    static void ClipTriangleWithOneVertexOut(std::vector<Triangle>& clippedTriangles, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Plane& plane);
    static void ClipTriangleWithTwoVertexOut(std::vector<Triangle>& clippedTriangles, const Vertex& v0, const Vertex& v1, const Vertex& v2, const Plane& plane);
    static bool IsBackFace(const Vector4& v0, const Vector4& v1, const Vector4& v2);
    static u32 CalculateClipCode(const Vector4& position);
	static Point2D CalculateViewPoint(const Vector4& position);

    static std::vector<Vertex> vertices;
};

}

#endif // !_RASTERIZER_H_
