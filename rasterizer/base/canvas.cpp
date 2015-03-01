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
	}
}

Canvas::~Canvas()
{
}

bool Canvas::SetPixel(int x, int y, const Color32& color)
{
	if (x < 0 || x >= width) return false;
	if (y < 0 || y >= height) return false;
	if (color.a == 0) return true;
	int offset = y * width + x;
	if (color.a == 255) pixels[offset] = color.rgba;
	else pixels[offset] = Color32::Lerp(Color32(pixels[offset]), color, color.a / 255.f).rgba;
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

void Canvas::BeginDraw()
{
	pixels.assign(width * height, Color32::black.rgba);
}

void Canvas::EndDraw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glFlush();
}



}