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
		float invZ = 1.0f;
		float invW = 1.0f;
	};

    struct VertexMini
    {
        Vector4 position = Vector4();
        u32 clipCode = 0x0;
        
        static VertexMini Lerp(const VertexMini& a, const VertexMini& b, float t)
        {
            assert(0.f <= t && t <= 1.f);
            
            VertexMini vertex;
            vertex.position = Vector4::Lerp(a.position, b.position, t);
            vertex.clipCode = CalculateClipCode(vertex.position);
            
            return vertex;
        }
    };
    
	struct Vertex
	{
		Vector4 position = Vector4();
		Vector3 normal = Vector3::up;
		Vector3 tangent = Vector3::right;
		Vector2 texcoord = Vector2::zero;
		Point2D point = Point2D();
        u32 clipCode = 0x0;
        
        static Vertex Lerp(const Vertex& a, const Vertex& b, float t)
        {
            assert(0.f <= t && t <= 1.f);
            
            Vertex vertex;
            vertex.position = Vector4::Lerp(a.position, b.position, t);
            vertex.normal = Vector3::Lerp(a.normal, b.normal, t);
            vertex.tangent = Vector3::Lerp(a.tangent, b.tangent, t);
            vertex.texcoord = Vector2::Lerp(a.texcoord, b.texcoord, t);
            vertex.clipCode = CalculateClipCode(vertex.position);
            
            return vertex;
        }
	};

    template<typename VertexType, int VertexCount>
	struct VertexSet
	{
		VertexType v[VertexCount];
	};
    
    typedef VertexSet<Vertex, 3> Triangle;
    typedef VertexSet<VertexMini, 2> Line;
    
    struct Plane
    {
        u32 cullMask = 0x0;
        typedef float (*ClippingFunc)(const Vector4& v0, const Vector4& v1);
        ClippingFunc clippingFunc = nullptr;
        Plane(u32 _cullMask, ClippingFunc _clippingFunc): cullMask(_cullMask), clippingFunc(_clippingFunc) {}
    };
    static Plane viewFrustumPlanes[6];

	static Vector3 lightDir;
	static Canvas* canvas;
	static CameraPtr camera;
	static MaterialPtr material;
    typedef Color (*FragmentShader)(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector4& interp);
	static FragmentShader fragmentShader;

    static void Initialize();
    
	static void DrawLine(int x0, int x1, int y0, int y1, const Color32& color);
	static void DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color);
	static void DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);
	static void DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color);

    static Color FS(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector4& interp);
    static void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
    static void DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color& color);

    template<typename VertexType>
    static std::vector<VertexType> ClipLine(const VertexType& v0, const VertexType& v1);
    template<typename VertexType>
    static std::vector<VertexType> ClipTriangle(const VertexType& v0, const VertexType& v1, const VertexType& v2);
    
private:
	static int Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p);
	static void Plot(int x, int y, const Color32& color);
	static void Plot(int x, int y, const Color32& color, float alpha, bool swapXY = false);

	static float Clip(float f0, float w0, float f1, float w1);
    
    template<typename VertexType>
    static void ClipLineFromPlane(std::vector<VertexType>& clippedLines,
                                  const VertexType& v0, const VertexType& v1, const Plane& plane);
    
    template<typename VertexType>
    static void ClipTriangleFromPlane(std::vector<VertexType>& clippedTriangles,
                                      const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane);

    template<typename VertexType>
    static void ClipTriangleWithOneVertexOut(std::vector<VertexType>& clippedTriangles,
                                             const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane);
    template<typename VertexType>
    static void ClipTriangleWithTwoVertexOut(std::vector<VertexType>& clippedTriangles,
                                             const VertexType& v0, const VertexType& v1, const VertexType& v2, const Plane& plane);
    
    template<typename Type>
    static Type TriangleInterpolation(const Type& v0, const Type& v1, const Type& v2, const Vector4& interp);
    
    static bool IsBackFace(const Vector4& v0, const Vector4& v1, const Vector4& v2);
    static u32 CalculateClipCode(const Vector4& position);
	static Point2D CalculateViewPoint(const Vector4& position);

    static std::vector<Vertex> vertices;
};

}

#endif // !_RASTERIZER_H_
