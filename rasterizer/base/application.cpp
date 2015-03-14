#include "application.h"
#include "canvas.h"

namespace rasterizer
{

Application* Application::GetInstance()
{
	static Application _GlobalApplication;
	return &_GlobalApplication;
}

bool Application::CreateApplication(const char* title, int width, int height)
{
    if (!glfwInit()) return false;

	glfwWindowHint(GLFW_DOUBLEBUFFER, 0);
	glfwWindowHint(GLFW_RESIZABLE, 0);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) return false;
    
    glfwMakeContextCurrent(window);
	
    this->width = width;
	this->height = height;
	return true;
}

Canvas* Application::GetCanvas()
{
	Canvas* canvas = new Canvas(width, height);
	return canvas;
}

void Application::RunLoop()
{
    assert(window != nullptr);
	while (!glfwWindowShouldClose(window))
    {
        if (loopFunc != nullptr) loopFunc();
		glfwPollEvents();
    }
    glfwTerminate();
}

float Application::GetTime()
{
    return (float) glfwGetTime();
}
    
}
