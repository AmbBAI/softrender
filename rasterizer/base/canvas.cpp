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
    
    glViewport(0, 0, width, height);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void Canvas::Present()
{
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glFlush();
}




}