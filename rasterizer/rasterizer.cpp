#include "rasterizer.h"

namespace rasterizer
{

Camera* Rasterizer::camera = nullptr;
Canvas* Rasterizer::canvas = nullptr;
Texture* Rasterizer::texture = nullptr;


rasterizer::Vector3 Rasterizer::lightDir = Vector3(-1.f, -1.f, -1.f).Normalize();

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
		Plot(x, y, color, steep);
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
	xgap = 1.f - FloatPart(x0 + 0.5);
	xpxl1 = Mathf::RoundToInt(xend);
	ypxl1 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(xpxl1, ypxl1, color, xgap * (1 - yfpart), steep);
	Plot(xpxl1, ypxl1 + 1, color, xgap * yfpart, steep);
	intery = yend + gradient;

	xend = Mathf::Round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = 1.f - FloatPart(x1 + 0.5);
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
	drawColor.a *= Mathf::Clamp01(alpha);
	Plot(x, y, drawColor, swapXY);
}

void Rasterizer::Plot(int x, int y, const Color32& color, bool swapXY)
{
	assert(canvas != nullptr);

	if (swapXY) Plot(y, x, color);
	else Plot(x, y, color);
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
		//printf("%s => %s => %s => %s => (%.2f %.2f)\n",
		//	mesh.vertices[i].ToString().c_str(),
		//	posW.ToString().c_str(),
		//	posC.ToString().c_str(),
		//	point.ToString().c_str(),
		//	x, y);
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

	std::vector<Point2D> points;
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		int x = Mathf::RoundToInt((point.x + 1) * width / 2);
		int y = Mathf::RoundToInt((point.y + 1) * height / 2);
		points.push_back(Point2D(x, y));
	}

	for (int i = 0; i + 2 < (int)mesh.indices.size(); i += 3)
	{
		int v0 = mesh.indices[i];
		int v1 = mesh.indices[i+1];
		int v2 = mesh.indices[i+2];

		DrawLine(points[v0].x, points[v1].x, points[v0].y, points[v1].y, color);
		DrawLine(points[v1].x, points[v2].x, points[v1].y, points[v2].y, color);
		DrawLine(points[v2].x, points[v0].x, points[v2].y, points[v0].y, color);
	}
}
//
//void Rasterizer::DrawTriangle(const Color32& color, const Vector2& v0, const Vector2& v1, const Vector2& v2, Canvas* canvas)
//{
//	int minX = Mathf::FloorToInt(Mathf::Min(v0.x, v1.x, v2.x));
//	int minY = Mathf::FloorToInt(Mathf::Min(v0.y, v1.y, v2.y));
//	int maxX = Mathf::FloorToInt(Mathf::Max(v0.x, v1.x, v2.x));
//	int maxY = Mathf::FloorToInt(Mathf::Max(v0.y, v1.y, v2.y));
//
//	float deltaX01 = v0.x - v1.x;
//	float deltaX12 = v1.x - v2.x;
//	float deltaX20 = v2.x - v0.x;
//
//	float deltaY01 = v0.y - v1.y;
//	float deltaY12 = v1.y - v2.y;
//	float deltaY20 = v2.y - v0.y;
//
//	Vector2 topLeft(minX, minY);
//	float w0Row = Orient2D(v0, v1, topLeft);
//	float w1Row = Orient2D(v1, v2, topLeft);
//	float w2Row = Orient2D(v2, v0, topLeft);
//
//	//if (deltaY01 < 0 || (deltaY01 == 0 && deltaX01 < 0)) w0Row += 1.0f;
//	//if (deltaY12 < 0 || (deltaY12 == 0 && deltaX12 < 0)) w1Row += 1.0f;
//	//if (deltaY20 < 0 || (deltaY20 == 0 && deltaX20 < 0)) w2Row += 1.0f;
//
//	for (int y = minY; y <= maxY; ++y)
//	{
//		float w0 = w0Row;
//		float w1 = w1Row;
//		float w2 = w2Row;
//
//		for (int x = minX; x <= maxX; ++x)
//		{
//			if (w0 > 0 && w1 > 0 && w2 > 0)
//			{
//				Plot(x, y, color);
//			}
//
//			w0 += deltaY01;
//			w1 += deltaY12;
//			w2 += deltaY20;
//		}
//
//		w0Row -= deltaX01;
//		w1Row -= deltaX12;
//		w2Row -= deltaX20;
//	}
//}
//

float Rasterizer::Orient2D(const Vector2& v1, const Vector2& v2, const Vector2& p)
{
	return (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x);
}

float Rasterizer::Orient2D(const Point2D& v1, const Point2D& v2, const Point2D& p)
{
	return (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x);
}


void Rasterizer::DrawTriangle(const Point2D& v0, const Point2D& v1, const Point2D& v2, const Color32& color)
{
	int minX = Mathf::Min(v0.x, v1.x, v2.x);
	int minY = Mathf::Min(v0.y, v1.y, v2.y);
	int maxX = Mathf::Max(v0.x, v1.x, v2.x);
	int maxY = Mathf::Max(v0.y, v1.y, v2.y);

	int deltaX01 = v0.x - v1.x;
	int deltaX12 = v1.x - v2.x;
	int deltaX20 = v2.x - v0.x;

	int deltaY01 = v0.y - v1.y;
	int deltaY12 = v1.y - v2.y;
	int deltaY20 = v2.y - v0.y;

	Point2D topLeft(minX, minY);
	int w0Row = Orient2D(v0, v1, topLeft);
	int w1Row = Orient2D(v1, v2, topLeft);
	int w2Row = Orient2D(v2, v0, topLeft);

	if (deltaY01 < 0 || (deltaY01 == 0 && deltaX01 < 0)) w0Row += 1.0f;
	if (deltaY12 < 0 || (deltaY12 == 0 && deltaX12 < 0)) w1Row += 1.0f;
	if (deltaY20 < 0 || (deltaY20 == 0 && deltaX20 < 0)) w2Row += 1.0f;

	for (int y = minY; y <= maxY; ++y)
	{
		int w0 = w0Row;
		int w1 = w1Row;
		int w2 = w2Row;

		for (int x = minX; x <= maxX; ++x)
		{
			if (w0 > 0 && w1 > 0 && w2 > 0)
			{
				float depth = v0.depth * w1 + v1.depth * w2 + v2.depth * w0;
				depth /= (w0 + w1 + w2);
				canvas->SetPixel(x, y, depth, color);
				//int depthColorVal = (1 - depth) * 80 * 255;
				//Color32 depthColor = Color32(depthColorVal, depthColorVal, depthColorVal, depthColorVal);
				//canvas->SetPixel(x, y, depth, depthColor);
			}

			w0 += deltaY01;
			w1 += deltaY12;
			w2 += deltaY20;
		}

		w0Row -= deltaX01;
		w1Row -= deltaX12;
		w2Row -= deltaX20;
	}
}

void Rasterizer::DrawMeshColor(const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera->GetViewMatrix();
	const Matrix4x4* projection = camera->GetProjectionMatrix();

	std::vector<Point2D> points;
	std::vector<Vector3> normals;
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		int x = Mathf::RoundToInt((point.x + 1) * width / 2);
		int y = Mathf::RoundToInt((point.y + 1) * height / 2);
		points.push_back(Point2D(x, y, point.z));

		Vector3 normal = transform.MultiplyVector(mesh.normals[i]);
		normals.push_back(normal);
	}

	for (int i = 0; i + 2 < (int)mesh.indices.size(); i += 3)
	{
		int v0 = mesh.indices[i];
		int v1 = mesh.indices[i + 1];
		int v2 = mesh.indices[i + 2];

		Color32 drawColor = ((i / 3) & 1) == 0 ? Color32(0x2088ffff) : Color32(0x20ff88ff);
		//Color32 drawColor = Color32::white;
		//Vector3 normal = normals[v0].Add(normals[v1]).Add(normals[v2]).Normalize();
		//drawColor.r = (normal.x + 1.f) / 2 * 255;
		//drawColor.g = (normal.y + 1.f) / 2 * 255;
		//drawColor.b = (normal.z + 1.f) / 2 * 255;
		DrawTriangle(points[v0], points[v1], points[v2], drawColor);
		//DrawTriangle(canvas, color, points[v0], points[v1], points[v2]);
	}
}

void Rasterizer::DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color& color)
{
	int minX = Mathf::Min(v0.vpPoint.x, v1.vpPoint.x, v2.vpPoint.x);
	int minY = Mathf::Min(v0.vpPoint.y, v1.vpPoint.y, v2.vpPoint.y);
	int maxX = Mathf::Max(v0.vpPoint.x, v1.vpPoint.x, v2.vpPoint.x);
	int maxY = Mathf::Max(v0.vpPoint.y, v1.vpPoint.y, v2.vpPoint.y);

	int deltaX01 = v0.vpPoint.x - v1.vpPoint.x;
	int deltaX12 = v1.vpPoint.x - v2.vpPoint.x;
	int deltaX20 = v2.vpPoint.x - v0.vpPoint.x;

	int deltaY01 = v0.vpPoint.y - v1.vpPoint.y;
	int deltaY12 = v1.vpPoint.y - v2.vpPoint.y;
	int deltaY20 = v2.vpPoint.y - v0.vpPoint.y;

	Point2D topLeft(minX, minY);
	int w0Row = Orient2D(v0.vpPoint, v1.vpPoint, topLeft);
	int w1Row = Orient2D(v1.vpPoint, v2.vpPoint, topLeft);
	int w2Row = Orient2D(v2.vpPoint, v0.vpPoint, topLeft);

	if (deltaY01 < 0 || (deltaY01 == 0 && deltaX01 < 0)) w0Row += 1.0f;
	if (deltaY12 < 0 || (deltaY12 == 0 && deltaX12 < 0)) w1Row += 1.0f;
	if (deltaY20 < 0 || (deltaY20 == 0 && deltaX20 < 0)) w2Row += 1.0f;

	for (int y = minY; y <= maxY; ++y)
	{
		int w0 = w0Row;
		int w1 = w1Row;
		int w2 = w2Row;

		for (int x = minX; x <= maxX; ++x)
		{
			if (w0 > 0 && w1 > 0 && w2 > 0)
			{
				float depth = v0.vpPoint.depth * w1 + v1.vpPoint.depth * w2 + v2.vpPoint.depth * w0;
				depth /= (w0 + w1 + w2);
				if (depth < canvas->GetDepth(x, y))
				{
					canvas->SetDepth(x, y, depth);

					Vector3 normal = v0.normal.Multiply(w1).Add(v1.normal.Multiply(w2)).Add(v2.normal.Multiply(w0)).Normalize();
					/*Vector3 tangent = v0.tangent.Multiply(w1).Add(v1.tangent.Multiply(w2)).Add(v2.tangent.Multiply(w0)).Normalize();

					float tw = 1;
					Vector3 binormal = tangent.Cross(normal).Multiply(tw).Normalize();
					tangent = normal.Cross(binormal).Normalize();*/

					Color32 normalColor(255, (normal.x + 1) / 2 * 255, (normal.y + 1) / 2 * 255, (normal.z + 1) / 2 * 255);
					//Color32 tangentColor(255, (tangent.x + 1) / 2 * 255, (tangent.y + 1) / 2 * 255, (tangent.z + 1) / 2 * 255);
					//canvas->SetPixel(x, y, depth, normalColor);
					canvas->SetPixel(x, y, depth, normalColor);
					//float lightVal = Mathf::Max(0, normal.Dot(lightDir.Negate()));
					//Color drawColor = color;// color.Multiply(lightVal);

					//if (texture)
					//{
					//	Vector2 uv = v0.texcoord.Multiply(w1).Add(v1.texcoord.Multiply(w2)).Add(v2.texcoord.Multiply(w0));
					//	uv = uv.Divide(w0 + w1 + w2);
					//	drawColor = drawColor.Modulate(texture->Sample(uv.x, uv.y));
					//}

					//canvas->SetPixel(x, y, drawColor);
					//canvas->SetPixel(x, y, depth, color);
					//int depthColorVal = (1 - depth) * 80 * 255;
					//Color32 depthColor = Color32(depthColorVal, depthColorVal, depthColorVal, depthColorVal);
					//canvas->SetPixel(x, y, depth, depthColor);
				}
			}

			w0 += deltaY01;
			w1 += deltaY12;
			w2 += deltaY20;
		}

		w0Row -= deltaX01;
		w1Row -= deltaX12;
		w2Row -= deltaX20;
	}
}

void Rasterizer::DrawMesh(const Mesh& mesh, const Matrix4x4& transform, const Color& color)
{
	assert(canvas != nullptr);
	assert(camera != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera->GetViewMatrix();
	const Matrix4x4* projection = camera->GetProjectionMatrix();

	// MVP
	int vertexN = (int)mesh.vertices.size();
	std::vector<Vertex> vertices;
	vertices.resize(vertexN);

	int i = 0;
//#pragma omp parallel for private(i)
	for (i = 0; i < vertexN; ++i)
	{
		Vertex vertex;

		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		vertex.position = posC;

		Vector3 normal = transform.MultiplyVector(mesh.normals[i]).Normalize();
		vertex.normal = normal;

		Vector3 tangent = transform.MultiplyVector(mesh.tangents[i]).Normalize();
		vertex.tangent = tangent;

		vertex.texcoord = mesh.texcoords[i];

		Vector3 posP = projection->MultiplyPoint(posC);
		vertex.projection = posP;

		vertex.vpPoint.x = Mathf::RoundToInt((posP.x + 1) * width / 2);
		vertex.vpPoint.y = Mathf::RoundToInt((posP.y + 1) * height / 2);
		vertex.vpPoint.depth = posP.z;

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

		Face face;
		face.v[0] = vertices[i0];
		face.v[1] = vertices[i1];
		face.v[2] = vertices[i2];
		
		if (Orient2D(face.v[0].vpPoint, face.v[1].vpPoint, face.v[2].vpPoint) <= 0) continue;
		//if (IsOutOfCamera(vertices[i0], vertices[i1], vertices[i2])) continue;

		DrawTriangle(face.v[0], face.v[1], face.v[2], color);
	}
}

}