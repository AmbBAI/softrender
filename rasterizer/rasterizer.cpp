#include "rasterizer.h"

namespace rasterizer
{

Canvas* Rasterizer::canvas = nullptr;
CameraPtr Rasterizer::camera;
rasterizer::MaterialPtr Rasterizer::material;
std::vector<Rasterizer::Vertex> Rasterizer::vertices;
Rasterizer::FragmentShader Rasterizer::fragmentShader = nullptr;

rasterizer::Vector3 Rasterizer::lightDir = Vector3(-1.f, -1.f, -1.f).Normalize();

Rasterizer::Plane Rasterizer::viewFrustumPlanes[6];

void Rasterizer::Initialize()
{
    viewFrustumPlanes[0].cullMask = 0x01;
    viewFrustumPlanes[0].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.x, -v0.w, v1.x, -v1.w); };

    viewFrustumPlanes[1].cullMask = 0x02;
	viewFrustumPlanes[1].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.x, v0.w, v1.x, v1.w); };

    viewFrustumPlanes[2].cullMask = 0x04;
	viewFrustumPlanes[2].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.y, -v0.w, v1.y, -v1.w);};

	viewFrustumPlanes[3].cullMask = 0x08;
	viewFrustumPlanes[3].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.y, v0.w, v1.y, v1.w); };

	viewFrustumPlanes[4].cullMask = 0x10;
	viewFrustumPlanes[4].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.z, -v0.w, v1.z, -v1.w); };

	viewFrustumPlanes[5].cullMask = 0x20;
	viewFrustumPlanes[5].clippingFunc =
		[](const Vector4& v0, const Vector4& v1) { return ClipLine(v0.z, v0.w, v1.z, v1.w); };
}
    
void TestColor(Canvas* canvas)
{
	int w = canvas->GetWidth();
	int h = canvas->GetHeight();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			canvas->SetPixel(x, y,
				Color(1.f, (float)x / w, (float)y / h, 0.f));
		}
	}
}

void Rasterizer::DrawLine(int x0, int x1, int y0, int y1, const Color32& color)
{
	bool steep = Mathf::Abs(y1 - y0) > Mathf::Abs(x1 - x0);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int deltaX = x1 - x0;
	int deltaY = Mathf::Abs(y1 - y0);
	int error = deltaX / 2;
	int yStep = -1;
	if (y0 < y1) yStep = 1;

	for (int x = x0, y = y0; x < x1; ++x)
	{
		steep ? Plot(y, x, color) : Plot(x, y, color);
		error = error - deltaY;
		if (error < 0)
		{
			y = y + yStep;
			error = error + deltaX;
		}
	}
}

void Rasterizer::DrawSmoothLine(float x0, float x1, float y0, float y1, const Color32& color)
{
	float deltaX = x1 - x0;
	float deltaY = y1 - y0;

	bool steep = Mathf::Abs(deltaX) < Mathf::Abs(deltaY);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		std::swap(deltaX, deltaY);
	}
	if (x1 < x0)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	float gradient = (float)deltaY / deltaX;

	float xend, yend, xgap, intery;
	int xpxl1, xpxl2, ypxl1, ypxl2;

	float yfpart;

	xend = Mathf::Round(x0);
	yend = y0 + gradient * (xend - x0);
	xgap = 1.f - FloatPart(x0 + 0.5f);
	xpxl1 = Mathf::RoundToInt(xend);
	ypxl1 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(xpxl1, ypxl1, color, xgap * (1 - yfpart), steep);
	Plot(xpxl1, ypxl1 + 1, color, xgap * yfpart, steep);
	intery = yend + gradient;

	xend = Mathf::Round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = 1.f - FloatPart(x1 + 0.5f);
	xpxl2 = Mathf::RoundToInt(xend);
	ypxl2 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(xpxl2, ypxl2, color, xgap * (1 - yfpart), steep);
	Plot(xpxl2, ypxl2 + 1, color, xgap * yfpart, steep);

	for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
	{
		int ipartIntery = IntPart(intery);
		float fpartIntery = FloatPart(intery);
		Plot(x, ipartIntery, color, 1.0f - fpartIntery, steep);
		Plot(x, ipartIntery + 1, color, fpartIntery, steep);

		intery += gradient;
	}

}

float Rasterizer::FloatPart(float v)
{
	return v - Mathf::Floor(v);
}

int Rasterizer::IntPart(float v)
{
	return Mathf::FloorToInt(v);
}

void Rasterizer::Plot(int x, int y, const Color32& color, float alpha, bool swapXY /*= false*/)
{
	assert(canvas != nullptr);

	Color32 drawColor = color;
	drawColor.a = (u8)(drawColor.a * Mathf::Clamp01(alpha));
	swapXY ? Plot(y, x, drawColor) : Plot(x, y, drawColor);
}

void Rasterizer::Plot(int x, int y, const Color32& color)
{
	assert(canvas != nullptr);

	canvas->SetPixel(x, y, color);
}

void Rasterizer::DrawMeshPoint(const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera->GetViewMatrix();
	const Matrix4x4* projection = camera->GetProjectionMatrix();

	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		float x = (point.x + 1) * width / 2;
		float y = (point.y + 1) * height / 2;
		Plot(Mathf::RoundToInt(x), Mathf::RoundToInt(y), color);
	}
}

void Rasterizer::DrawMeshWireFrame(const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera->GetViewMatrix();
	const Matrix4x4* projection = camera->GetProjectionMatrix();
    Matrix4x4 mv = (*view).Multiply(transform);

	std::vector<Point2D> points;
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posC = mv.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 point = projection->MultiplyPoint(posC);
		int x = Mathf::RoundToInt((point.x + 1) * width / 2);
		int y = Mathf::RoundToInt((point.y + 1) * height / 2);
		points.push_back(Point2D(x, y));
	}

	for (int i = 0; i + 2 < (int)mesh.indices.size(); i += 3)
	{
		int v0 = mesh.indices[i];
		int v1 = mesh.indices[i + 1];
		int v2 = mesh.indices[i + 2];

		DrawLine(points[v0].x, points[v1].x, points[v0].y, points[v1].y, color);
		DrawLine(points[v1].x, points[v2].x, points[v1].y, points[v2].y, color);
		DrawLine(points[v2].x, points[v0].x, points[v2].y, points[v0].y, color);
	}
}

int Rasterizer::Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p)
{
	return (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x);
}

void Rasterizer::DrawTriangle(const Face& f)
{
	const Point2D& p0 = f.v[0].point;
	const Point2D& p1 = f.v[1].point;
	const Point2D& p2 = f.v[2].point;

	int minX = Mathf::Min(p0.x, p1.x, p2.x);
	int minY = Mathf::Min(p0.y, p1.y, p2.y);
	int maxX = Mathf::Max(p0.x, p1.x, p2.x);
	int maxY = Mathf::Max(p0.y, p1.y, p2.y);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();
	if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= width) maxX = width - 1;
	if (maxY >= height) maxY = height - 1;
//
//	if (maxX < minX) return;WWW
//	if (maxY < minY) return;

	int dx01 = p0.x - p1.x;
	int dx12 = p1.x - p2.x;
	int dx20 = p2.x - p0.x;

	int dy01 = p0.y - p1.y;
	int dy12 = p1.y - p2.y;
	int dy20 = p2.y - p0.y;

	Point2D topLeft(minX, minY);
	int startW0 = Orient2D(p0, p1, topLeft);
	int startW1 = Orient2D(p1, p2, topLeft);
	int startW2 = Orient2D(p2, p0, topLeft);

	if (dy01 < 0 || (dy01 == 0 && dx01 < 0)) startW0 += 1;
	if (dy12 < 0 || (dy12 == 0 && dx12 < 0)) startW1 += 1;
	if (dy20 < 0 || (dy20 == 0 && dx20 < 0)) startW2 += 1;

	float invZ0 = 1.f / p0.w;
	float invZ1 = 1.f / p1.w;
	float invZ2 = 1.f / p2.w;

    //printf("%d-%d %d-%d\n", minY, maxY, minX, maxX);
	for (int y = minY; y <= maxY; ++y)
	{
		int w0 = startW0;
		int w1 = startW1;
		int w2 = startW2;

		for (int x = minX; x <= maxX; ++x)
		{
			if (w0 > 0 && w1 > 0 && w2 > 0)
			{
				float wz0 = w1 * invZ0;
				float wz1 = w2 * invZ1;
				float wz2 = w0 * invZ2;
				float invW = 1.0f / (wz0 + wz1 + wz2);


				float depth = p0.depth * wz0 + p1.depth * wz1 + p2.depth * wz2;
				depth *= invW;
                if (depth > 0.0f && depth < 1.0f && depth < canvas->GetDepth(x, y))
				{
					canvas->SetDepth(x, y, depth);
					//canvas->SetPixel(x, y, Color(1.f, 1.f - depth, 1.f - depth, 1.f - depth));
					if (fragmentShader != nullptr)
					{
						Color color = fragmentShader(f, wz0, wz1, wz2, invW);
						canvas->SetPixel(x, y, color);
					}
				}
			}

			w0 += dy01;
			w1 += dy12;
			w2 += dy20;
		}

		startW0 -= dx01;
		startW1 -= dx12;
		startW2 -= dx20;
	}
}

Color Rasterizer::FS(const Face& face, float w0, float w1, float w2, float invW)
{
	const Vertex& v0 = face.v[0];
	const Vertex& v1 = face.v[1];
	const Vertex& v2 = face.v[2];

	Vector3 normal = (v0.normal * w0 + v1.normal * w1 + v2.normal * w2).Normalize();
	Vector2 uv = (v0.texcoord * w0 + v1.texcoord * w1 + v2.texcoord * w2) * invW;

	Color color = Color::white;
	MaterialPtr material = Rasterizer::material;

	if (material && material->diffuseTexture)
	{
		color = color.Modulate(material->diffuseTexture->Sample(uv.x, uv.y));
	}

	if (material && material->normalTexture)
	{
		Vector3 tangent = (v0.tangent * w0 + v1.tangent * w1 + v2.tangent * w2).Normalize();
		Vector3 binormal = normal.Cross(tangent).Normalize();
		Matrix4x4 tbn = Matrix4x4::TBN(tangent, binormal, normal);

		Color normalColor = material->normalTexture->Sample(uv.x, uv.y);
		normal = Vector3(normalColor.r * 2 - 1, normalColor.g * 2 - 1, normalColor.b * 2 - 1);

		normal = tbn.MultiplyVector(normal);
		//normalColor = Color(1, (normal.x + 1) / 2, (normal.y + 1) / 2, (normal.z + 1) / 2);
	}

	float lightVal = Mathf::Max(0.f, normal.Dot(lightDir.Negate()));
	color = color.Multiply(lightVal);
	return color;
}

void Rasterizer::DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color& color)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera->GetViewMatrix();
	const Matrix4x4* projection = camera->GetProjectionMatrix();
    Matrix4x4 mv = (*view).Multiply(transform);

	// MVP
	int vertexN = (int)mesh.vertices.size();
	vertices.resize(vertexN);

	int i = 0;
//#pragma omp parallel for private(i)
	for (i = 0; i < vertexN; ++i)
	{
		Vertex vertex;

		Vector3 position = mv.MultiplyPoint3x4(mesh.vertices[i]);
		vertex.normal = transform.MultiplyVector(mesh.normals[i]).Normalize();
		vertex.tangent = transform.MultiplyVector(mesh.tangents[i]).Normalize();
		vertex.texcoord = mesh.texcoords[i];
		vertex.position = projection->MultiplyPoint(position);
		vertex.clipCode = CalculateClipCode(vertex.position);

		vertices[i] = vertex;
	}

	// Primitive Assembly
	int faceN = (int)mesh.indices.size() / 3;
//#pragma omp parallel for private(i)
	for (i = 0; i < faceN; ++i)
	{
		int i0 = mesh.indices[i * 3];
		int i1 = mesh.indices[i * 3 + 1];
		int i2 = mesh.indices[i * 3 + 2];
        
        // TODO backface cull
        
        auto faces = ClipTriangle(vertices[i0], vertices[i1], vertices[i2]);
        for (auto f : faces)
        {
			f.v[0].point = CalculateViewPoint(f.v[0].position);
			f.v[1].point = CalculateViewPoint(f.v[1].position);
			f.v[2].point = CalculateViewPoint(f.v[2].position);
            DrawTriangle(f);
        }
	}

}
    
std::vector<Rasterizer::Face> Rasterizer::ClipTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
    std::vector<Face> faces;
    if (0 != (v0.clipCode & v1.clipCode & v2.clipCode)) return faces;
    
    Face face;
    face.v[0] = v0;
    face.v[1] = v1;
    face.v[2] = v2;
	faces.push_back(face);
    
    for (auto p : viewFrustumPlanes)
    {
        std::vector<Face> clippedFaces;
        for (auto f : faces)
        {
            auto outFaces = ClipTriangleFromPlane(f, p);
            clippedFaces.insert(clippedFaces.end(), outFaces.begin(), outFaces.end());
        }
        faces = clippedFaces;
    }
    return faces;
}

std::vector<Rasterizer::Face> Rasterizer::ClipTriangleFromPlane(const Face& face, const Plane& plane)
{
    std::vector<Face> outFaces;
    const Vertex& v0 = face.v[0];
    const Vertex& v1 = face.v[1];
    const Vertex& v2 = face.v[2];
    
    if (0 == ((v0.clipCode | v1.clipCode | v2.clipCode) & plane.cullMask))
    {
        outFaces.push_back(face);
    }
    else if ((~v0.clipCode) & v1.clipCode & v2.clipCode & plane.cullMask)
    {
		float t1 = plane.clippingFunc(v1.position, v0.position);
		float t2 = plane.clippingFunc(v2.position, v0.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		Face face;
		face.v[0] = v0;
		face.v[1] = ClipLineFromPlane(t1, v1, v0);
		face.v[2] = ClipLineFromPlane(t2, v2, v0);
		outFaces.push_back(face);
	}
    else if (v0.clipCode & (~v1.clipCode) & v2.clipCode & plane.cullMask)
    {
		float t0 = plane.clippingFunc(v0.position, v1.position);
		float t2 = plane.clippingFunc(v2.position, v1.position);
		assert(0.f <= t0 && t0 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		Face face;
		face.v[0] = v1;
		face.v[1] = ClipLineFromPlane(t2, v2, v1);
		face.v[2] = ClipLineFromPlane(t0, v0, v1);
		outFaces.push_back(face);
    }
    else if (v0.clipCode & v1.clipCode & (~v2.clipCode) & plane.cullMask)
    {
		float t1 = plane.clippingFunc(v1.position, v2.position);
		float t0 = plane.clippingFunc(v0.position, v2.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t0 && t0 <= 1.f);
		Face face;
		face.v[0] = v2;
		face.v[1] = ClipLineFromPlane(t0, v0, v2);
		face.v[2] = ClipLineFromPlane(t1, v1, v2);
		outFaces.push_back(face);
    }
    else if ((~v0.clipCode) & (~v1.clipCode) & v2.clipCode & plane.cullMask)
    {
		float t1 = plane.clippingFunc(v2.position, v1.position);
		float t0 = plane.clippingFunc(v2.position, v0.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t0 && t0 <= 1.f);
		Vertex extVertex0 = ClipLineFromPlane(t1, v2, v1);
		Vertex extVertex1 = ClipLineFromPlane(t0, v2, v0);
		Face face;
		face.v[0] = v0;
		face.v[1] = v1;
		face.v[2] = extVertex0;
		outFaces.push_back(face);
		face.v[0] = v0;
		face.v[1] = extVertex0;
		face.v[2] = extVertex1;
		outFaces.push_back(face);
    }
    else if (v0.clipCode & (~v1.clipCode) & (~v2.clipCode) & plane.cullMask)
    {
		float t1 = plane.clippingFunc(v0.position, v1.position);
		float t2 = plane.clippingFunc(v0.position, v2.position);
		assert(0.f <= t1 && t1 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		Vertex extVertex0 = ClipLineFromPlane(t2, v0, v2);
		Vertex extVertex1 = ClipLineFromPlane(t1, v0, v1);
		Face face;
		face.v[0] = v1;
		face.v[1] = v2;
		face.v[2] = extVertex0;
		outFaces.push_back(face);
		face.v[0] = v1;
		face.v[1] = extVertex0;
		face.v[2] = extVertex1;
		outFaces.push_back(face);
    }
    else if ((~v0.clipCode) & v1.clipCode & (~v2.clipCode) & plane.cullMask)
    {
		float t0 = plane.clippingFunc(v1.position, v0.position);
		float t2 = plane.clippingFunc(v1.position, v2.position);
		assert(0.f <= t0 && t0 <= 1.f);
		assert(0.f <= t2 && t2 <= 1.f);
		Vertex extVertex0 = ClipLineFromPlane(t0, v1, v0);
		Vertex extVertex1 = ClipLineFromPlane(t2, v1, v2);
		Face face;
		face.v[0] = v2;
		face.v[1] = v0;
		face.v[2] = extVertex0;
		outFaces.push_back(face);
		face.v[0] = v2;
		face.v[1] = extVertex0;
		face.v[2] = extVertex1;
		outFaces.push_back(face);
    }
    return outFaces;
}

float Rasterizer::ClipLine(float f0, float w0, float f1, float w1)
{
	return (w0 - f0) / ((w0 - f0) - (w1 - f1));
}

Rasterizer::Vertex Rasterizer::ClipLineFromPlane(float t, const Vertex& v0, const Vertex& v1)
{
	assert(0.f <= t && t <= 1.f);

	Vertex vertex;
	vertex.position = Vector4::Lerp(v0.position, v1.position, t);
	vertex.normal = Vector3::Lerp(v0.normal, v1.normal, t);
	vertex.tangent = Vector3::Lerp(v0.tangent, v1.tangent, t);
	vertex.texcoord = Vector2::Lerp(v0.texcoord, v1.texcoord, t);
	vertex.clipCode = CalculateClipCode(vertex.position);

	return vertex;
}

u32 Rasterizer::CalculateClipCode(const Vector4& position)
{
	float w = position.w;
	u32 clipCode = 0x0;
	if (position.x < -w) clipCode |= viewFrustumPlanes[0].cullMask;
	if (position.x > w) clipCode |= viewFrustumPlanes[1].cullMask;
	if (position.y < -w) clipCode |= viewFrustumPlanes[2].cullMask;
	if (position.y > w) clipCode |= viewFrustumPlanes[3].cullMask;
	if (position.z < -w) clipCode |= viewFrustumPlanes[4].cullMask;
	if (position.z > w) clipCode |= viewFrustumPlanes[5].cullMask;
	return clipCode;
}

Rasterizer::Point2D Rasterizer::CalculateViewPoint(const Vector4& position)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	float w = position.w;
	float invW = 1.f / w;

	Point2D point;
	point.x = Mathf::RoundToInt(((position.x * invW) + 1.f) / 2.f * width);
	point.y = Mathf::RoundToInt(((position.y * invW) + 1.f) / 2.f * height);
	point.depth = (position.z * invW + 1.f) / 2.f;
	point.w = w;
	return point;
}


}