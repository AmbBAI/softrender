#ifndef _BASE_APPLICATION_H_
#define _BASE_APPLICATION_H_

#include "header.h"

namespace rasterizer
{

class Canvas;
class Application
{
	Application();

public:
	static Application* GetInstance();
	typedef void(*LoopFunc)();
	bool CreateApplication(int argc, char *argv[], const char* title, int width, int height);

	void SetRunLoop(LoopFunc loopFunc) { this->loopFunc = loopFunc; }
	void RunLoop();

	Canvas* GetCanvas();

private:
	LoopFunc	loopFunc;

	int width;
	int height;

	bool isConsoleVisible;
};

}

#endif // !_BASE_APPLICATION_H_
