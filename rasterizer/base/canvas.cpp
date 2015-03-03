#include "canvas.h"
#include "color.h"

namespace rasterizer
{

Canvas::Canvas(int width, int height)
{
	this->width = width;
	this->height = height;

	if (width > 0 && height > 0)
	{
		pixels.assign(width * height, Color32::black.rgba);
		depths.assign(width * height, 1.0f);
	}
}

Canvas::~Canvas()
{
}

bool Canvas::SetPixel(int x, int y, const Color32& color)
{
	if (x < 0 || x >= width) return false;
	if (y < 0 || y >= height) return false;

	int offset = y * width + x;
	pixels[offset] = color.rgba;
	return true;
}

bool Canvas::SetPixel(int x, int y, float depth, const Color32& color)
{
	if (x < 0 || x >= width) return false;
	if (y < 0 || y >= height) return false;

	int offset = y * width + x;
	if (depths[offset] < depth) return true;
	depths[offset] = depth;
	pixels[offset] = color.rgba;
	return true;
}

bool Canvas::SetPixel(int x, int y, const Color& color)
{
	return SetPixel(x, y, Color32(color));
}

int Canvas::GetWidth()
{
	return width;
}

int Canvas::GetHeight()
{
	return height;
}

void Canvas::Clear()
{
	pixels.assign(width * height, Color32::black.rgba);
	depths.assign(width * height, 1.0f);
}

void Canvas::Present()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glFlush();
}



}