#ifndef _BASE_CANVAS_H_
#define _BASE_CANVAS_H_

#include "header.h"
#include "application.h"
#include "color.h"

namespace rasterizer
{

class Canvas
{
	friend Application;
	Canvas(int width, int height);
	~Canvas() = default;

public:
	bool SetPixel(int x, int y, const Color& color);
	bool SetPixel(int x, int y, float depth, const Color& color);
	bool SetPixel(int x, int y, const Color32& color);
	float GetDepth(int x, int y);
	void SetDepth(int x, int y, float depth);
	
	int GetWidth();
	int GetHeight();

	void Clear();
	void Present();

private:
	int width;
	int height;

	std::vector<u32> pixels;
	std::vector<float> depths;
};

}

#endif //!_BASE_CANVAS_H_
