#include "application.h"
#include "canvas.h"
#include "input.h"

namespace rasterizer
{

Application* Application::GetInstance()
{
	static Application _GlobalApplication;
	return &_GlobalApplication;
}

Application::~Application()
{
	if (window != nullptr)
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}

	if (canvas != nullptr)
	{
		delete canvas;
		canvas = nullptr;
	}

	if (input != nullptr)
	{
		delete input;
		input = nullptr;
	}
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
	assert(width > 0 && height > 0);
	if (canvas == nullptr)
	{
		canvas = new Canvas(width, height);
	}
	return canvas;
}

Input* Application::GetInput()
{
	assert(window != nullptr);
	if (input == nullptr)
	{
		input = new Input(window);
	}
	return input;
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
