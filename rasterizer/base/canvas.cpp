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
		pixels = new uint32_t[width * height];
		depths = new float[width * height];
	}
}

Canvas::~Canvas()
{
	if (pixels != nullptr)
	{
		delete[] pixels;
		pixels = nullptr;
	}

	if (depths != nullptr)
	{
		delete[] depths;
		depths = nullptr;
	}
}

void Canvas::Clear()
{
	assert(pixels != nullptr);
	assert(depths != nullptr);

	std::fill(pixels, pixels + width * height, 0x0);
	std::fill(depths, depths + width * height, 1.f);
    
    glViewport(0, 0, width, height);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void Canvas::Present()
{
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glFlush();
}




}