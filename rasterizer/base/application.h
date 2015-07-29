#ifndef _BASE_APPLICATION_H_
#define _BASE_APPLICATION_H_

#include "header.h"

namespace rasterizer
{

class Input;
class Application
{
	Application() = default;
	~Application();

public:
	static Application* GetInstance();
	typedef void(*LoopFunc)();
	bool CreateApplication(const char* title, int width, int height);
	void SetTitle(const char* title);

	void SetRunLoop(LoopFunc loopFunc) { this->loopFunc = loopFunc; }
	void RunLoop();
    
    float GetTime();
	float GetDeltaTime();

	Input* GetInput();

private:
	LoopFunc	loopFunc = nullptr;

    GLFWwindow* window = nullptr;
    
	int width = 0;
	int height = 0;

	Input* input = nullptr;

	float lastFrameTime;
	float thisFrameTime;
	float deltaTime;
};

}

#endif // !_BASE_APPLICATION_H_
