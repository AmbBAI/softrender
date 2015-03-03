#include "rasterizer.h"

namespace rasterizer
{

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

void Rasterizer::DrawLine(Canvas* canvas, const Color32& color, int x0, int x1, int y0, int y1)
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
		Plot(canvas, x, y, color, steep);
		error = error - deltaY;
		if (error < 0)
		{
			y = y + yStep;
			error = error + deltaX;
		}
	}
}

void Rasterizer::DrawSmoothLine(Canvas* canvas, const Color32& color, float x0, float x1, float y0, float y1)
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
	Plot(canvas, xpxl1, ypxl1, color, xgap * (1 - yfpart), steep);
	Plot(canvas, xpxl1, ypxl1 + 1, color, xgap * yfpart, steep);
	intery = yend + gradient;

	xend = Mathf::Round(x1);
	yend = y1 + gradient * (xend - x1);
	xgap = 1.f - FloatPart(x1 + 0.5);
	xpxl2 = Mathf::RoundToInt(xend);
	ypxl2 = IntPart(yend);
	yfpart = FloatPart(yend);
	Plot(canvas, xpxl2, ypxl2, color, xgap * (1 - yfpart), steep);
	Plot(canvas, xpxl2, ypxl2 + 1, color, xgap * yfpart, steep);

	for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
	{
		int ipartIntery = IntPart(intery);
		float fpartIntery = FloatPart(intery);
		Plot(canvas, x, ipartIntery, color, 1.0f - fpartIntery, steep);
		Plot(canvas, x, ipartIntery + 1, color, fpartIntery, steep);

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

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color, float alpha, bool swapXY /*= false*/)
{
	assert(canvas != nullptr);

	Color32 drawColor = color;
	drawColor.a *= Mathf::Clamp01(alpha);
	Plot(canvas, x, y, drawColor, swapXY);
}

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color, bool swapXY)
{
	assert(canvas != nullptr);

	if (swapXY) Plot(canvas, y, x, color);
	else Plot(canvas, x, y, color);
}

void Rasterizer::Plot(Canvas* canvas, int x, int y, const Color32& color)
{
	assert(canvas != nullptr);

	canvas->SetPixel(x, y, color);
}

void Rasterizer::DrawMeshPoint(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		float x = (point.x + 1) * width / 2;
		float y = (point.y + 1) * height / 2;
		Plot(canvas, Mathf::RoundToInt(x), Mathf::RoundToInt(y), color);
		//printf("%s => %s => %s => %s => (%.2f %.2f)\n",
		//	mesh.vertices[i].ToString().c_str(),
		//	posW.ToString().c_str(),
		//	posC.ToString().c_str(),
		//	point.ToString().c_str(),
		//	x, y);
	}
}

void Rasterizer::DrawMeshWireFrame(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

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

		DrawLine(canvas, color, points[v0].x, points[v1].x, points[v0].y, points[v1].y);
		DrawLine(canvas, color, points[v1].x, points[v2].x, points[v1].y, points[v2].y);
		DrawLine(canvas, color, points[v2].x, points[v0].x, points[v2].y, points[v0].y);
	}
}
//
//void Rasterizer::DrawTriangle(Canvas* canvas, const Color32& color, const Vector2& v0, const Vector2& v1, const Vector2& v2)
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
//				Plot(canvas, x, y, color);
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


void Rasterizer::DrawTriangle(Canvas* canvas, const Color32& color, const Point2D& v0, const Point2D& v1, const Point2D& v2)
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

void Rasterizer::DrawMeshColor(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

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
		DrawTriangle(canvas, drawColor, points[v0], points[v1], points[v2]);
		//DrawTriangle(canvas, color, points[v0], points[v1], points[v2]);
	}
}

void Rasterizer::DrawTriangle(Canvas* canvas, const Color32& color, const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
	int minX = Mathf::Min(v0.point.x, v1.point.x, v2.point.x);
	int minY = Mathf::Min(v0.point.y, v1.point.y, v2.point.y);
	int maxX = Mathf::Max(v0.point.x, v1.point.x, v2.point.x);
	int maxY = Mathf::Max(v0.point.y, v1.point.y, v2.point.y);

	int deltaX01 = v0.point.x - v1.point.x;
	int deltaX12 = v1.point.x - v2.point.x;
	int deltaX20 = v2.point.x - v0.point.x;

	int deltaY01 = v0.point.y - v1.point.y;
	int deltaY12 = v1.point.y - v2.point.y;
	int deltaY20 = v2.point.y - v0.point.y;

	Point2D topLeft(minX, minY);
	int w0Row = Orient2D(v0.point, v1.point, topLeft);
	int w1Row = Orient2D(v1.point, v2.point, topLeft);
	int w2Row = Orient2D(v2.point, v0.point, topLeft);

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
				float depth = v0.point.depth * w1 + v1.point.depth * w2 + v2.point.depth * w0;
				depth /= (w0 + w1 + w2);
				Vector3 normal = v0.normal.Multiply(w1).Add(v1.normal.Multiply(w2)).Add(v2.normal.Multiply(w0)).Normalize();
				Color32 normalColor(255, (normal.x + 1) / 2 * 255, (normal.y + 1) / 2 * 255, (normal.z + 1) / 2 * 255);
				canvas->SetPixel(x, y, depth, normalColor);
				//canvas->SetPixel(x, y, depth, color);
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


void Rasterizer::DrawMesh(Canvas* canvas, const Camera& camera, const Mesh& mesh, const Matrix4x4& transform, const Color32& color)
{
	assert(canvas != nullptr);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	const Matrix4x4* view = camera.GetViewMatrix();
	const Matrix4x4* projection = camera.GetProjectionMatrix();

	std::vector<Vertex> points;
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vertex vertex;

		Vector3 posW = transform.MultiplyPoint3x4(mesh.vertices[i]);
		Vector3 posC = view->MultiplyPoint(posW);
		Vector3 point = projection->MultiplyPoint(posC);
		int x = Mathf::RoundToInt((point.x + 1) * width / 2);
		int y = Mathf::RoundToInt((point.y + 1) * height / 2);
		vertex.point = Point2D(x, y, point.z);

		Vector3 normal = transform.MultiplyVector(mesh.normals[i]);
		vertex.normal = normal;

		points.push_back(vertex);
	}

	for (int i = 0; i + 2 < (int)mesh.indices.size(); i += 3)
	{
		int v0 = mesh.indices[i];
		int v1 = mesh.indices[i + 1];
		int v2 = mesh.indices[i + 2];

		DrawTriangle(canvas, color, points[v0], points[v1], points[v2]);
	}
}

}