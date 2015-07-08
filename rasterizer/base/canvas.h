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
	~Canvas();

public:
	Color32 GetPixel(int x, int y)
	{
		if (x < 0 || x >= width) return Color32::black;
		if (y < 0 || y >= height) return Color32::black;

		int offset = y * width + x;
		return Color32(pixels[offset]);
	}

	bool SetPixel(int x, int y, const Color& color)
    {
        return SetPixel(x, y, Color32(color));
    }
    
    bool SetPixel(int x, int y, const Color32& color)
    {
        if (x < 0 || x >= width) return false;
        if (y < 0 || y >= height) return false;
        
        int offset = y * width + x;
        pixels[offset] = color.rgba;
        return true;
    }
    
	float GetDepth(int x, int y)
    {
        if (x < 0 || x >= width) return 1.f;
        if (y < 0 || y >= height) return 1.f;
            
        int offset = y * width + x;
        return depths[offset];
    }
	void SetDepth(int x, int y, float depth)
    {
        if (x < 0 || x >= width) return;
        if (y < 0 || y >= height) return;
        
        int offset = y * width + x;
        depths[offset] = depth;
    }
	
    int GetWidth() { return width; }
    int GetHeight() { return height; }

	void Clear();
	void Present();

private:
	int width;
	int height;

	uint32_t* pixels = nullptr;
	float* depths = nullptr;
};

}

#endif //!_BASE_CANVAS_H_
