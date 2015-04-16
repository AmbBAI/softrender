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
		pixels.assign(width * height, 0);
		depths.assign(width * height, 1.0f);
	}
}

void Canvas::Clear()
{
	pixels.assign(width * height, 0);
	depths.assign(width * height, 1.f);
}

void Canvas::Present()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glFlush();
}




}