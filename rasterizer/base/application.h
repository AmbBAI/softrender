#ifndef _BASE_APPLICATION_H_
#define _BASE_APPLICATION_H_

#include "header.h"

namespace rasterizer
{

class Canvas;
class Application
{
	Application() = default;

public:
	static Application* GetInstance();
	typedef void(*LoopFunc)();
	bool CreateApplication(const char* title, int width, int height);

	void SetRunLoop(LoopFunc loopFunc) { this->loopFunc = loopFunc; }
	void RunLoop();
    
    float GetTime();

	Canvas* GetCanvas();

private:
	LoopFunc	loopFunc = nullptr;

    GLFWwindow* window = nullptr;
    
	int width = 0;
	int height = 0;
};

}

#endif // !_BASE_APPLICATION_H_
